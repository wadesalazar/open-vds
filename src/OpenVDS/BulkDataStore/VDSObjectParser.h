#pragma once

#include <cctype>
#include <string>
#include <vector>
#include <stack>
#include <memory>
#include "HueBulkDataStore.h"
#include "HueBulkDataStoreFormat.h"

#define CREATEOBJECT "CreateObj"

#pragma warning(push)
#pragma warning(disable:4996)
#include "json/json.h"
#pragma warning(pop)

#ifdef _MSC_VER
#define snprintf _snprintf
#endif


/*
        BNF Grammar for VDSObject structure
        ____________________________________
        <VDSOBJ>            -> <OBJECTDESCRPTION><MAINOBJECTBODY>
        <MAINOBJECTBODY>    -> {<SUBBODY>}
        <SUBBODY>           -> <KEYVALUE><SUBBODY>  | <OBJECTDESCRPTION><KEYVALUEBODY><SUBBODY> | EMPTY
        <KEYVALUEBODY>      -> {<KEYVALUESEQ>}
        <KEYVALUE>          -> (<LEFTNAME>=<VALUE>|<LEFTNAME>:<SUBOBJECT>)|EMPTY
        <KEYVALUESEQ>       -> <KEYVALUE><KEYVALUESEQ>|EMPTY
        <OBJECTDESCRIPTION> -> CreateObj(<ObjType>,<ChildListName>,<ID>)
        <OBJTYPENAME>       -> <LEFTNAME>
        <CHILDLISTNAME>     -> <LEFTNAME>
        <ID>                -> <POSITIVENUMBER>
        <LEFTNAME>          -> (<ALPHANUMERIC>|_)+
        <VALUE>             -> <PNNUMBER> | <QUOTEDSTRING> | <BLOBVALUE>
        <BLOBVALUE>         -> N:<DIGITSEQ>:<ALPHANUMERIC>+
        <PNNUMBER>          -> <NUMBERPREFIX>(<DIGITSEQ> | <DIGITSEQ>.<DIGITSEQ> ((e|E)(+|-)<DIGITSEQ>|EMPTY) )
        <NUMBERPREFIX>      -> +|-|DIGIT|.
        <DIGITSEQ>          -> (0|1|2|3|4|5|6|7|8|9)+
        <QUOTEDSTRING>      -> "<ALPHANUMERIC>+|_|.|[|]"
        <SUBOBJECT>         -> {(<POSITIVENUMBER>,n({<KEYVALUESEQ>})} | {<KEYVALUESEQ>} | EMPTY
        <POSITIVENUMBER>    -> <DIGITSEQ>
*/

typedef class ParserState
{
private:
  const char *_tokenStart, *_tokenEnd;
  bool foundNumber;
  bool foundFractionalNumber;
  bool foundQuotedString;
  char* key;
  char* stringVal;
  Json::Value & objectRoot;
public:
  ParserState(Json::Value &objectRoot) :
    _tokenStart(nullptr), _tokenEnd(nullptr),
    foundNumber(false),
    foundFractionalNumber(false),
    foundQuotedString(false),
    key(nullptr),
    stringVal(nullptr),
    objectRoot(objectRoot)
  {}

  ~ParserState()
  {
    if (key != NULL)
    {
      free(key);
    }
    if (stringVal != NULL)
    {
      free(stringVal);
    }
  }

  Json::Value &getObjectRoot()
  {
    return objectRoot;
  }

  // Token extractors 

  bool isTokenNumber()
  {
    return foundNumber;
  }
  bool isTokenFractionalNumber()
  {
    return foundFractionalNumber;
  }
  bool isTokenQuotedString()
  {
    return foundQuotedString;
  }
  bool IsEmpty() const
  {
    return _tokenStart == _tokenEnd;
  }

  void extractKey()
  {
    size_t length = (_tokenEnd - _tokenStart) + 1;
    if (key != nullptr)
    {
      free(key);
    }

    key = static_cast<char *>(malloc(length));
    memset(key, 0, length);
    if (*_tokenStart != '\"')
    {
      memcpy(key, _tokenStart, length - 1);
    }
    else
    {
      memcpy(key, _tokenStart + 1, length - 3);
    }
  }

  void extractStringVal()
  {
    if (stringVal != nullptr)
    {
      free(stringVal);
    }

    size_t length = 0;
    if (isTokenNumber())
    {
      length = (_tokenEnd - _tokenStart) + 1;
      stringVal = static_cast<char *>(malloc(length));
      memset(stringVal, 0, length);
      memcpy(stringVal, _tokenStart, length - 1);
    }
    else if (isTokenQuotedString())
    {
      length = (_tokenEnd - _tokenStart) / sizeof(char) + 1 - 2;
      stringVal = static_cast<char *>(malloc(length));
      memset(stringVal, 0, length);
      memcpy(stringVal, _tokenStart + 1, length - 1);
    }
  }

  char * getKey()
  {
    return key;
  }

  char * getStringVal()
  {
    return stringVal;
  }


  // Token Manipulators 

  bool getNextToken(const char ** buffer);
  bool matchTokenArray(const char* pToken);
  bool matchSingle(const char token);
  const char *getTokenStart()
  {
    return _tokenStart;
  }
  const char *getTokenEnd()
  {
    return _tokenEnd;
  }

  bool getAndMatchTokenArray(const char ** buffer, const char* pToken);
  bool getAndMatchSingleToken(const char ** buffer, const char token);

} *ParserState_pc, **ParserState_ppc;


class Parser
{
private:
  std::unique_ptr<ParserState> ps;
  HueBulkDataStore::Buffer * chunkBuffer;
  std::unique_ptr<Json::Value> root;
  const char *buffer;
public:
  Parser()
  {
    chunkBuffer = nullptr;
    root = std::unique_ptr<Json::Value>(new Json::Value(Json::objectValue));
    ps = std::unique_ptr<ParserState>(new ParserState(*root.get()));
    buffer = nullptr;
  }
  ~Parser()
  {
    if (chunkBuffer != nullptr)
    {
      HueBulkDataStore::ReleaseBuffer(chunkBuffer);
    }
  }
  bool ReadSerializedVDSObject(const char *vdsFileName);
  ParserState* getParserState()
  {
    return ps.get();
  }
  const char* getBuffer()
  {
    if (chunkBuffer != nullptr)
    {
      buffer = ((char *)chunkBuffer->Data());
      return buffer;
    }
    else if (buffer != nullptr)
    {
      return buffer;
    }
    return nullptr;
  }
  void setBuffer(const char* parseString)
  {
    buffer = parseString;
  }
  Json::Value *getRoot() const
  {
    return root.get();
  }
};


// Refer to BNF above
// The function names are capitalized to reflect the grammar Non-terminals
bool VDSObj(ParserState *, const char **, Json::Value &);
bool MainObjectBody(ParserState *, const char **, Json::Value &);
bool ObjectDescription(ParserState *, const char **, bool &, Json::Value &, bool extractPersistentID=false);
bool SubBody(ParserState *, const char **, Json::Value &);
bool KeyValueBody(ParserState *, const char **, Json::Value &);
bool KeyValue(ParserState *, const char **, Json::Value &);
bool KeyValueSeq(ParserState *, const char **, Json::Value &);
bool SubObject(ParserState *, const char **, Json::Value &);
bool Value(ParserState *, const char **, Json::Value &);
bool ObjTypeName(ParserState *, const char **);
bool ChildListName(ParserState *, const char **);
bool Id(ParserState *, const char **, Json::Value &, bool extractPersistentID=false);
long long PositiveNumber(ParserState *, const char **);
bool LeftName(ParserState *, const char **);
bool PNNumber(ParserState *, const char **, Json::Value &);
bool QuotedString(ParserState *, const char **, Json::Value &);
bool BLOBValue(ParserState *, const char **, Json::Value &);


bool parseChildCreateObj(ParserState *, const char **, Json::Value &);

extern "C"
{
  HUEBDS_EXPORTS bool InitializeParser(const char * vdsFileName, Parser **parser, const char * parseString);
  HUEBDS_EXPORTS void DeInitializeParser(Parser **parser);
  HUEBDS_EXPORTS bool ParseVDS(Parser **parser);
  HUEBDS_EXPORTS size_t GetVDSObjectJsonString(const Parser *parser, char *jsonStringBuffer, size_t bufferSize);
  HUEBDS_EXPORTS size_t GetVolumeDataLayoutJsonString(const Parser *parser, char *jsonStringBuffer, size_t bufferSize);
}
