#include "VDSObjectParser.h"
#include "Base64.h"
#include <sstream>
#include <locale.h>
#include <stdint.h>

#include <fmt/format.h>

#ifdef _WIN32
typedef _locale_t locale_t;
#define strtod_l _strtod_l
#define strtof_l _strtof_l
#define strtoll  _strtoi64
#define strtoull _strtoui64
#endif

static locale_t &
GetCLocale()
{
#ifdef _WIN32
  static locale_t hCLocale = _create_locale(LC_CTYPE, "C");
#else
  static locale_t hCLocale = newlocale(LC_CTYPE_MASK, "C", NULL);
#endif
  return hCLocale;
}

bool InitializeParser(const char * vdsFileName, Parser **parser, const char * parseString = "")
{
  *parser = new Parser();

  if (strlen(parseString) == 0)
  {
    return (*parser)->ReadSerializedVDSObject(vdsFileName);
  }
  else
  {
    (*parser)->setBuffer(parseString);
  }

  return true;
}

bool ParseVDS(Parser **parser)
{
  if (*parser == nullptr)
  {
    return false;
  }
  const char *buffer = (*parser)->getBuffer();
  if (buffer == nullptr)
  {
    return false;
  }
  return VDSObj((*parser)->getParserState(), &buffer, *(*(parser))->getRoot());
}

bool Parser::ReadSerializedVDSObject(const char *vdsFileName)
{
  HueBulkDataStore * dataStore = nullptr;
  HueBulkDataStore::FileInterface *fileInterface = nullptr;

  dataStore = HueBulkDataStore::Open(vdsFileName);
  if (dataStore == nullptr)
  {
    return false;
  }

  fileInterface = dataStore->OpenFile("VDSObject");
  if (fileInterface == NULL)
  {
    HueBulkDataStore::Close(dataStore);
    return false;
  }

  chunkBuffer = fileInterface->ReadChunkData(0);

  if (chunkBuffer == nullptr || chunkBuffer->Size() == 0)
  {
    HueBulkDataStore::Close(dataStore);
    return false;
  }

  if (dataStore != nullptr)
  {
    HueBulkDataStore::Close(dataStore);
  }
  return true;
}

void DeInitializeParser(Parser **parser)
{
  if (*parser != nullptr)
  {
    delete (*parser);
  }
}

size_t GetVDSObjectJsonString(const Parser *parser, char *jsonStringBuffer, size_t bufferSize)
{
  Json::StreamWriterBuilder swb;
  swb["commentStyle"] = "None";
  swb["indentation"] = " ";
  Json::StreamWriter *writer = swb.newStreamWriter();
  std::ostringstream oss;
  writer->write(*parser->getRoot(), &oss);

  if (jsonStringBuffer)
  {
    strncpy(jsonStringBuffer, oss.str().c_str(), bufferSize);
  }

  return oss.str().size();
}

Json::Value TranslateVolumeDataLayoutDescriptor(Json::Value const & root)
{
  int brickSize = 0;
  sscanf(root["FullVCSize"].asCString(), "HUEVCSIZE_%d", &brickSize);

  int lodLevels = 0;
  sscanf(root["LODLevels"].asCString(), "LODLevel%d", &lodLevels);

  Json::Value layoutDescriptorJson;

  layoutDescriptorJson["brickSize"] = fmt::format("BrickSize_{}", brickSize);
  layoutDescriptorJson["negativeMargin"] = root["NegativeMargin"].asInt();
  layoutDescriptorJson["positiveMargin"] = root["PositiveMargin"].asInt();
  layoutDescriptorJson["brickSize2DMultiplier"] = 4; // FIXME: Check if object type is VDSSpread and set to 1 in that case
  layoutDescriptorJson["lodLevels"] = (lodLevels == 0) ? std::string("LODLevels_None") : fmt::format("LODLevels_{}", lodLevels);
  layoutDescriptorJson["create2DLODs"] = (root["Create2DLODs"].asString() != "FALSE");
  layoutDescriptorJson["forceFullResolutionDimension"] = (root["ForceFullResolutionDimension"].asString() != "FALSE");
  layoutDescriptorJson["fullResolutionDimension"] = root["FullResolutionDimension"].asInt();
  return layoutDescriptorJson;
}

Json::Value TranslateAxisDescriptors(Json::Value const & root)
{
  int dimensionality = 0;
  sscanf(root["Dimensions"].asCString(), "HUEDIMENSION_%d", &dimensionality);

  Json::Value axisDescriptorsJson(Json::arrayValue);

  for(int dimension = 0; dimension < dimensionality; dimension++)
  {
    Json::Value axisDescriptorJson;

    axisDescriptorJson["numSamples"]    = root[fmt::format("Dimension{}Size", dimension)].asInt();
    axisDescriptorJson["name"]          = root[fmt::format("Dimension{}Name", dimension)].asString();
    axisDescriptorJson["unit"]          = root[fmt::format("Dimension{}Unit", dimension)].asString();
    axisDescriptorJson["coordinateMin"] = root[fmt::format("Dimension{}Coordinate", dimension)]["Min"].asFloat();
    axisDescriptorJson["coordinateMax"] = root[fmt::format("Dimension{}Coordinate", dimension)]["Max"].asFloat();

    axisDescriptorsJson.append(axisDescriptorJson);
  }

  return axisDescriptorsJson;
}

std::string TranslateVCVoxelFormat(std::string const &vcVoxelFormat)
{
  const char vcVoxelFormatPrefix[] = "HUEVOXELFORMAT_";
  const size_t prefixLength = sizeof(vcVoxelFormatPrefix) - 1;

  if(vcVoxelFormat.substr(0, prefixLength) == vcVoxelFormatPrefix)
  {
    std::string format = vcVoxelFormat.substr(prefixLength);
    return (format == "1BIT") ? std::string("Format_1Bit") : fmt::format("Format_{}", format);
  }

  assert(0 && "Illegal format"); return "";
}

Json::Value TranslateChannelDescriptor(Json::Value const & root, bool isPrimaryChannel)
{
  int components = 0;
  sscanf(root[isPrimaryChannel ? "VoxelComponents" : "Components"].asCString(), "HUEVOXELCOMPONENTS_%d", &components);

  Json::Value valueRangeJson(Json::arrayValue);

  valueRangeJson.append(root["ValueRange"]["Min"].asFloat());
  valueRangeJson.append(root["ValueRange"]["Max"].asFloat());

  Json::Value channelDescriptorJson;

  channelDescriptorJson["format"] = TranslateVCVoxelFormat(root[isPrimaryChannel ? "VoxelFormat" : "Format"].asString());
  channelDescriptorJson["components"] = fmt::format("Components_{}", components);
  channelDescriptorJson["name"] = root[isPrimaryChannel ? "ValueName" : "Name"].asString();
  channelDescriptorJson["unit"] = root[isPrimaryChannel ? "ValueUnit" : "Unit"].asString();
  channelDescriptorJson["valueRange"] = valueRangeJson;
  channelDescriptorJson["channelMapping"] = isPrimaryChannel ? "Direct" : (root["ChannelMapping"].asUInt64() == 1976800267773298824ULL ? "PerTrace" : "Direct");
  channelDescriptorJson["mappedValues"] = isPrimaryChannel ? 0 : root["MappedValues"].asInt();
  channelDescriptorJson["discrete"] = (root["DiscreteData"].asString() != "FALSE");
  channelDescriptorJson["renderable"] = isPrimaryChannel ? true : (root["Renderable"].asString() != "FALSE");
  channelDescriptorJson["allowLossyCompression"] = isPrimaryChannel ? !(root["DiscreteData"].asString() != "FALSE") : (root["AllowLossyCompression"].asString() != "FALSE");
  channelDescriptorJson["useNoValue"] = (root["UseNoValue"].asString() != "FALSE");
  channelDescriptorJson["noValue"] = root["NoValue"].asFloat();
  channelDescriptorJson["integerScale"] = root["IntegerScale"].asFloat();
  channelDescriptorJson["integerOffset"] = root["IntegerOffset"].asFloat();

  return channelDescriptorJson;
}

Json::Value TranslateChannelDescriptors(Json::Value const & root)
{
  Json::Value channelDescriptorsJson(Json::arrayValue);

  channelDescriptorsJson.append(TranslateChannelDescriptor(root, true));

  Json::Value volumeDataChannelDescriptorArray = root["VolumeDataChannelDescriptor"];

  if(volumeDataChannelDescriptorArray.isArray())
  {
    for(int channel = 0; channel < (int)volumeDataChannelDescriptorArray.size(); channel++)
    {
      channelDescriptorsJson.append(TranslateChannelDescriptor(volumeDataChannelDescriptorArray[channel], false));
    }
  }
  return channelDescriptorsJson;
}

int DecodeHexChar(char hexChar)
{
  return (hexChar >= '0' && hexChar <= '9') ? hexChar - '0' + 0   :
         (hexChar >= 'a' && hexChar <= 'f') ? hexChar - 'a' + 0xa :
         (hexChar >= 'A' && hexChar <= 'F') ? hexChar - 'A' + 0xA : -1;
}

bool HexStringDecode(const char *data, int64_t len, std::vector<unsigned char> &result)
{
  bool error = false;

  result.reserve(result.size() + len / 2);

  while(len > 0)
  {
    int hi = DecodeHexChar(*data++); len--; if(hi == -1 || len == 0) { error = true; break; }
    int lo = DecodeHexChar(*data++); len--; if(lo == -1            ) { error = true; break; }
    int value = (hi << 4) | (lo);
    result.push_back(value);
  }

  return !error;
}

Json::Value TranslateBLOB(Json::Value const & root)
{
  if(!root.isString()) return Json::Value();

  const char *str, *cend;
  root.getString(&str, &cend);

  std::vector<uint8_t> blob;
  HexStringDecode(str, cend - str, blob);

  std::vector<char> base64;
  Base64Encode(blob.data(), blob.size(), base64);

  return Json::Value(&base64[0], &base64[0] + base64.size());
}

Json::Value TranslateVector(Json::Value const & root)
{
  Json::Value vectorJson(Json::arrayValue);

  const char* element[] = { "X", "Y", "Z", "T" };

  int i = 0;
  switch(root.size())
  {
  case 4: vectorJson.append(root[element[i++]]); // falls through
  case 3: vectorJson.append(root[element[i++]]); // falls through
  case 2: vectorJson.append(root[element[i++]]);
          vectorJson.append(root[element[i++]]);
  }

  return vectorJson;
}

Json::Value TranslateMetadata(Json::Value const & root)
{
  Json::Value
    metadataListJson(Json::arrayValue);

  for(auto const & metadata : root["MetadataList"])
  {
    Json::Value
      metadataJson(Json::objectValue);

    metadataJson["type"] = metadata["Type"].asString();
    metadataJson["category"] = metadata["Category"].asString();
    metadataJson["name"] = metadata["Name"].asString();

    if(metadata["Type"].asString().find("Vector") != std::string::npos)
    {
      metadataJson["value"] = TranslateVector(metadata["Value"]);
    }
    else if(metadata["Type"].asString() == "BLOB")
    {
      metadataJson["value"] = TranslateBLOB(metadata["Value"]);
    }
    else // scalar or string value
    {
      metadataJson["value"] = metadata["Value"];
    }

    metadataListJson.append(metadataJson);
  }

  return metadataListJson;
}

Json::Value TranslateVolumeDataLayout(Json::Value const & root)
{
  Json::Value volumeDataLayoutJson;

  volumeDataLayoutJson["layoutDescriptor"] = TranslateVolumeDataLayoutDescriptor(root);
  volumeDataLayoutJson["axisDescriptors"] = TranslateAxisDescriptors(root);
  volumeDataLayoutJson["channelDescriptors"] = TranslateChannelDescriptors(root);
  volumeDataLayoutJson["metadata"] = TranslateMetadata(root);

  return volumeDataLayoutJson;
}

size_t GetVolumeDataLayoutJsonString(const Parser *parser, char *jsonStringBuffer, size_t bufferSize)
{
  std::vector<uint8_t>
    result;

  Json::StreamWriterBuilder wbuilder;
  wbuilder["indentation"] = "    ";
  std::string document = Json::writeString(wbuilder, TranslateVolumeDataLayout(*parser->getRoot()));

  // strip carriage return
  result.reserve(document.length());
  for(char c : document)
  {
    if(c != '\r')
    {
      result.push_back(c);
    }
  }

  if (jsonStringBuffer)
  {
    memcpy(jsonStringBuffer, result.data(), std::min(bufferSize, result.size()));
    if(result.size() < bufferSize)
    {
      // fill remaining space in buffer with zeroes to be compatible with snprintf/strncpy
      memset(jsonStringBuffer + result.size(), 0, bufferSize - result.size());
    }
  }

  return result.size();
}

bool ParserState::getNextToken(const char ** buffer)
{
  foundNumber = false;
  foundFractionalNumber = false;
  foundQuotedString = false;
  const char *pbuffer = *buffer;
  _tokenStart = nullptr;
  _tokenEnd = nullptr;

  while (isspace(*pbuffer))
  {
    pbuffer++;
  }

  if (*pbuffer)
  {
    _tokenStart = pbuffer;

    switch (*pbuffer)
    {
    case ':':
      pbuffer++;
      if (*pbuffer == ':')
      { // Namespace
        pbuffer++;
      }
      break;
      // Single character token
    case '{': case '}': case ',': case '.': case '=': case ';': case '|':
    case '!': case '#': case '%': case '&': case '/': case '(': case ')':
      pbuffer++;
      break;
      // Negative number or "->"
    case '-':
      if (pbuffer[1] == '>')
      {
        pbuffer += 2;
        break;
      }
      // Falls through
    case '+':
      pbuffer++;

      if (!isdigit(*pbuffer))
      {
        break;
      }
      // Falls through
      // Number
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
      foundNumber = true;

      while (isdigit(*pbuffer))
      {
        pbuffer++;
      }
      // Match fractional part
      if (*pbuffer == '.')
      {
        foundFractionalNumber = true;

        if (!isdigit(pbuffer[1]))
        {
          break;
        }
        pbuffer++;

        while (isdigit(*pbuffer))
        {
          pbuffer++;
        }
      }
      // Match exponent
      if (*pbuffer == 'E' || *pbuffer == 'e')
      {
        foundFractionalNumber = true;

        if (!isdigit(pbuffer[1]))
        {
          if ((pbuffer[1] == '+' || pbuffer[1] == '-') && isdigit(pbuffer[2]))
          {
            pbuffer++;
          }
          else
          {
            break;
          }
        }
        pbuffer++;

        while (isdigit(*pbuffer))
        {
          pbuffer++;
        }
      }
      break;
      // Quoted string
    case '\"':
      pbuffer++;
      while (*pbuffer != '\"')
      {
        if (*pbuffer == '\0' || (pbuffer[0] == '\\' && pbuffer[1] == '\0'))
        {
          foundQuotedString = false;
          return false;
        }
        else if (*pbuffer == '\\')
        {
          pbuffer++;
        }
        pbuffer++;
      }
      pbuffer++;
      foundQuotedString = true;
      break;
      // Alphanumeric string
    default:
      pbuffer++;
      while (isalpha(*pbuffer) || isdigit(*pbuffer) || *pbuffer == '_' || *pbuffer == '.' || *pbuffer == '[' || *pbuffer == ']')
      {
        pbuffer++;
      }
      break;
    }
    _tokenEnd = pbuffer;
    *buffer = pbuffer;
    return true;
  }
  return false;
}


bool ParserState::matchTokenArray(const char* pToken)
{
  size_t nTokenLength = strlen(pToken);

  if ((size_t)(_tokenEnd - _tokenStart) == nTokenLength)
  {
    return strncmp(_tokenStart, pToken, nTokenLength) == 0;
  }

  return false;
}

bool ParserState::matchSingle(const char token)
{

  if (_tokenEnd == _tokenStart + 1)
  {
    if (_tokenStart[0] == token)
    {
      return true;
    }
  }

  return false;
}

bool ParserState::getAndMatchTokenArray(const char ** buffer, const char * pToken)
{
  return getNextToken(buffer) && matchTokenArray(pToken);
}

bool ParserState::getAndMatchSingleToken(const char ** buffer, const char  token)
{
  return getNextToken(buffer) && matchSingle(token);
}

// Context Free Grammar defined in Header. Each of the non-terminals in the grammar are defined as functions updating the ParserState 
// when parsing the buffer. The root is updated along the way. As a side-effect of parsing a JSON parse is constructed. This parse tree can
// subsequently be used to extract attributes using the [] notation. The JSON parse tree is constructed using the primitives and helpers from the jsoncpp 
// library (included as cpp/h).

bool VDSObj(ParserState *ps, const char **buffer, Json::Value &root)
{
  if (!ps->getNextToken(buffer))
  {
    return false;
  }
  if (ps->matchTokenArray(CREATEOBJECT))
  {
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
    bool isChildKeySet = false;
    if (!ObjectDescription(ps, buffer, isChildKeySet, root, true))
    {
      return false;
    }
    return  MainObjectBody(ps, buffer, root);
  }
  return false;
}

bool MainObjectBody(ParserState *ps, const char **buffer, Json::Value &root)
{
  if (!ps->getNextToken(buffer))
  {
    return false;
  }
  if (!ps->matchSingle('{'))
  {
    return false;
  }

  if (!ps->getNextToken(buffer))
  {
    return false;
  }

  if (ps->matchSingle('}'))
  {
    return true;
  }

  else if (!SubBody(ps, buffer, root))
  {
    return false;
  }

  if (!ps->matchSingle('}'))
  {
    return false;
  }

  return true;
}

bool ObjectDescription(ParserState *ps, const char **buffer, bool &childKeySet, Json::Value &root, bool extractPersistentID)
{
  if (!ps->matchSingle('('))
  {
    return false;
  }
  if (!ps->getNextToken(buffer))
  {
    return false;
  }
  if (!ObjTypeName(ps, buffer))
  {
    return false;
  }
  if (!ps->getAndMatchSingleToken(buffer, ','))
  {
    return false;
  }
  if (!ps->getNextToken(buffer))
  {
    return false;
  }
  if (!ChildListName(ps, buffer))
  {
    return false;
  }

  // Root token
  ps->extractKey();
  if (strcmp(ps->getKey(), "") != 0)
  {
    childKeySet = true;
  }

  if (!ps->getAndMatchSingleToken(buffer, ','))
  {
    return false;
  }
  if (!ps->getNextToken(buffer))
  {
    return false;
  }
  if (!Id(ps, buffer, root, extractPersistentID))
  {
    return false;
  }
  if (!ps->getAndMatchSingleToken(buffer, ')'))
  {
    return false;
  }
  return true;
}

bool SubBody(ParserState *ps, const char **buffer, Json::Value & root)
{
  if (ps->matchTokenArray(CREATEOBJECT))
  {
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
    bool childKeySet = false;
    if (!ObjectDescription(ps, buffer, childKeySet, root))
    {
      return false;
    }
    if (childKeySet)
    {
      if (!parseChildCreateObj(ps, buffer, root))
      {
        return false;
      }
    }
    else
    {
      if (!KeyValueBody(ps, buffer, root))
      {
        return false;
      }
    }
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
    return SubBody(ps, buffer, root);
  }
  else if (KeyValue(ps, buffer, root))
  {
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
    if (ps->matchSingle('}'))
    {
      return true;
    }

    return SubBody(ps, buffer, root);
  }

  return true;
}

bool parseChildCreateObj(ParserState *ps, const char **buffer, Json::Value &root)
{
  std::vector<char> key(ps->getKey(), ps->getKey() + strlen(ps->getKey()) + 1);

  if (ps->getObjectRoot().find(&key[0], &key[0] + strlen(&key[0])) == NULL)
  {
    ps->getObjectRoot()[ps->getKey()] = Json::arrayValue;
  }

  Json::Value parsedValue = Json::objectValue;
  if (!KeyValueBody(ps, buffer, parsedValue))
  {
    return false;
  }
  else
  {
    ps->getObjectRoot()[&key[0]].append(parsedValue);
  }
  return true;
}

bool KeyValueBody(ParserState *ps, const char **buffer, Json::Value &root)
{
  if (!ps->getNextToken(buffer))
  {
    return false;
  }
  if (!ps->matchSingle('{'))
  {
    return false;
  }

  if (!ps->getNextToken(buffer))
  {
    return false;
  }

  if (ps->matchSingle('}'))
  {
    return true;
  }

  KeyValueSeq(ps, buffer, root);


  if (!ps->matchSingle('}'))
  {
    return false;
  }

  return true;
}

bool KeyValue(ParserState *ps, const char **buffer, Json::Value &root)
{
  if (LeftName(ps, buffer))
  {
    ps->extractKey();

    if (!ps->getNextToken(buffer))
    {
      return false;
    }

    if (ps->matchSingle('='))
    {
      if (!ps->getNextToken(buffer))
      {
        return false;
      }
      if (!Value(ps, buffer, root))
      {
        return false;
      }
      if (!ps->getNextToken(buffer))
      {
        return false;
      }
      return ps->matchSingle(';');
    }
    else if (ps->matchSingle(':'))
    {
      if (!ps->getNextToken(buffer))
      {
        return false;
      }
      if (!SubObject(ps, buffer, root))
      {
        return false;
      }
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }

  return true;
}

bool KeyValueSeq(ParserState *ps, const char ** buffer, Json::Value & root)
{
  // This funtion will only return false if there is an error getting the next token.
  // According to the grammar, this function is called when matching a SUBOBJECT non-terminal and is 
  // always followed by a match of '}'.
  while (KeyValue(ps, buffer, root))
  {
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
  }
  return true;
}


bool LeftName(ParserState *ps, const char **buffer)
{
  const char *pc = ps->getTokenStart();
  while (pc != ps->getTokenEnd())
  {
    if (isalpha(*pc) || isdigit(*pc) || *pc == '_')
    {
      pc++;
      continue;
    }
    return false;
  }

  return true;
}


bool SubObject(ParserState *ps, const char ** buffer, Json::Value &root)
{
  if (!ps->matchSingle('{'))
  {
    return false;
  }

  if (!ps->getNextToken(buffer))
  {
    return false;
  }

  long long n = PositiveNumber(ps, buffer);
  if (n > 0)
  {
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
    if (!ps->matchSingle(','))
    {
      return false;
    }

    std::vector<char> arrayKey;
    if (strcmp(ps->getKey(), "") != 0)
    {
      root[ps->getKey()] = Json::arrayValue;
      size_t length = strlen(ps->getKey());
      arrayKey.resize(length + 1);
      memset(&arrayKey[0], 0, length + 1);
      strncpy(&arrayKey[0], ps->getKey(), length);
    }
    int i = 0;
    while (n > 0)
    {
      if (!ps->getNextToken(buffer))
      {
        return false;
      }
      if (!ps->matchSingle('{'))
      {
        return false;
      }
      if (!ps->getNextToken(buffer))
      {
        return false;
      }

      KeyValueSeq(ps, buffer, root[&arrayKey[0]][i++]);

      if (!ps->matchSingle('}'))
      {
        return false;
      }
      if (n != 1)
      {
        if (!ps->getAndMatchSingleToken(buffer, ','))
        {
          return false;
        }
      }
      n -= 1;
    }
    if (n > 0)
    {
      return false;
    }
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
    if (!ps->matchSingle('}'))
    {
      return false;
    }
  }
  else
  {
    if (strcmp(ps->getKey(), "") != 0)
    {
      root[ps->getKey()] = Json::objectValue;
    }

    KeyValueSeq(ps, buffer, root[ps->getKey()]);
    if (!ps->matchSingle('}'))
    {
      return false;
    }
  }
  return true;
}

bool Value(ParserState *ps, const char ** buffer, Json::Value &root)
{
  return BLOBValue(ps, buffer, root) || PNNumber(ps, buffer, root) || QuotedString(ps, buffer, root);
}

bool PNNumber(ParserState *ps, const char ** buffer, Json::Value &root)
{
  bool parseRet = ps->isTokenNumber();
  if (parseRet)
  {
    ps->extractStringVal();
    if(ps->isTokenFractionalNumber())
    {
      root[ps->getKey()] = Json::Value(strtod_l(ps->getStringVal(), NULL, GetCLocale()));
    }
    else
    {
      if(ps->getStringVal()[0] == '-' || ps->getStringVal()[0] == '+')
      {
        root[ps->getKey()] = Json::Value((int64_t)strtoll(ps->getStringVal(), NULL, 10));
      }
      else
      {
        root[ps->getKey()] = Json::Value((uint64_t)strtoull(ps->getStringVal(), NULL, 10));
      }
    }
  }
  return parseRet;
}

bool QuotedString(ParserState *ps, const char **buffer, Json::Value &root)
{
  bool parseRet = ps->isTokenQuotedString();
  if (parseRet)
  {
    ps->extractStringVal();
    root[ps->getKey()] = Json::Value(ps->getStringVal());
  }
  return parseRet;
}

bool BLOBValue(ParserState *ps, const char **buffer, Json::Value &root)
{
  if (ps->matchSingle('N') && *(buffer[0]) == ':')
  {
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
    if (!ps->matchSingle(':'))
    {
      return false;
    }
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
    long long skipBytes = PositiveNumber(ps, buffer) * 2;
    if (skipBytes < 0)
    {
      return false;
    }
    if (!ps->getNextToken(buffer))
    {
      return false;
    }
    if (!ps->matchSingle(':'))
    {
      return false;
    }
    if (skipBytes == 0)
    {
      return true;
    }

    root[ps->getKey()] = Json::Value(std::string(*buffer, skipBytes));
    *buffer += skipBytes;

    return true;
  }
  return false;
}

bool ObjTypeName(ParserState *ps, const char ** buffer)
{
  return ps->isTokenQuotedString();
}

bool ChildListName(ParserState *ps, const char ** buffer)
{
  return ps->isTokenQuotedString();
}

bool Id(ParserState *ps, const char ** buffer, Json::Value &root, bool extractPersistentID)
{
  long long pn = PositiveNumber(ps, buffer);
  if (pn >= 0)
  {
    if (extractPersistentID)
    {
      root["PersistentID"] = std::to_string(pn);
    }
    return true;
  }
  return false;
}

long long PositiveNumber(ParserState *ps, const char ** buffer)
{
  const char *pc = ps->getTokenStart();
  while (pc != ps->getTokenEnd())
  {
    if (!isdigit(*pc))
    {
      return -1;
    }
    pc++;
  }

  size_t tokenLength = (size_t)(ps->getTokenEnd() - ps->getTokenStart());
  std::vector<char> token;
  token.resize(tokenLength+1);
  strncpy(&(token[0]), ps->getTokenStart(), tokenLength);
  token[tokenLength] = 0;
  return std::stoll(&(token[0]));
}
