#include <fmt/printf.h>
#include <json/json.h>

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayout.h>

#include "Base64.h"

#include "cxxopts.hpp"
#include <PrintHelpers.h>

namespace OpenVDS
{
  extern Json::Value SerializeVolumeDataLayoutDescriptor(VolumeDataLayout const &volumeDataLayout);
  extern Json::Value SerializeAxisDescriptors(VolumeDataLayout const &volumeDataLayout);
  extern Json::Value SerializeChannelDescriptors(VolumeDataLayout const &volumeDataLayout);
}

const char ebcdic_to_ascii[256] =
{
  /*   0*/ 0, 0, 0, 0, 0, 0, 0, 0,
  /*   8*/ 0, 0, 0, 0, 0, '\r', 0, 0,
  /*  16*/ 0, 0, 0, 0, 0, 0, 0, 0,
  /*  24*/ 0, 0, 0, 0, 0, 0, 0, 0,
  /*  32*/ 0, 0, 0, 0, 0, '\n', 0, 0,
  /*  40*/ 0, 0, 0, 0, 0, 0, 0, 0,
  /*  48*/ 0, 0, 0, 0, 0, 0, 0, 0,
  /*  56*/ 0, 0, 0, 0, 0, 0, 0, 0,
  /*  64*/ ' ', 0, 0, 0, 0, 0, 0, 0,
  /*  72*/ 0, 0, 0, '.', '<', '(', '+', 0,
  /*  80*/ '&', 0, 0, 0, 0, 0, 0, 0,
  /*  88*/ 0, 0, '!', '$', '*', ')', ';', 0,
  /*  96*/ '-', '/', 0, 0, 0, 0, 0, 0,
  /* 104*/ 0, 0, '|', ',', '%', '_', '>', '?',
  /* 112*/ 0, 0, 0, 0, 0, 0, 0, 0,
  /* 120*/ 0, 0, ':', '#', '@', '\'', '=', '"',
  /* 128*/ 0, 'a', 'b', 'c', 'd', 'e', 'f', 'g',
  /* 136*/ 'h', 'i', 0, 0, 0, 0, 0, 0,
  /* 144*/ 0, 'j', 'k', 'l', 'm', 'n', 'o', 'p',
  /* 152*/ 'q', 'r', 0, 0, 0, 0, 0, 0,
  /* 160*/ 0, '~', 's', 't', 'u', 'v', 'w', 'x',
  /* 168*/ 'y', 'z', 0, 0, 0, 0, 0, 0,
  /* 176*/ 0, 0, 0, 0, 0, 0, 0, 0,
  /* 184*/ 0, '`', 0, 0, 0, 0, 0, 0,
  /* 192*/ '{', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  /* 200*/ 'H', 'I', 0, 0, 0, 0, 0, 0,
  /* 208*/ '}', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  /* 216*/ 'Q', 'R', 0, 0, 0, 0, 0, 0,
  /* 224*/ '\\', 0, 'S', 'T', 'U', 'V', 'W', 'X',
  /* 232*/ 'Y', 'Z', 0, 0, 0, 0, 0, 0,
  /* 240*/ '0', '1', '2', '3', '4', '5', '6', '7',
  /* 248*/ '8', '9', 0, 0, 0, 0, 0, 0,
};

static std::string MetadataTypeToString(OpenVDS::MetadataType type)
{
  switch (type)
  {
  case OpenVDS::MetadataType::Int:
    return "Int";
  case OpenVDS::MetadataType::IntVector2:
    return "IntVector2";
  case OpenVDS::MetadataType::IntVector3:
    return "IntVector3";
  case OpenVDS::MetadataType::IntVector4:
    return "IntVector4";
  case OpenVDS::MetadataType::Float:
    return "Float";
  case OpenVDS::MetadataType::FloatVector2:
    return "FloatVector2";
  case OpenVDS::MetadataType::FloatVector3:
    return "FloatVector3";
  case OpenVDS::MetadataType::FloatVector4:
    return "FloatVector4";
  case OpenVDS::MetadataType::Double:
    return "Double";
  case OpenVDS::MetadataType::DoubleVector2:
    return "DoubleVector2";
  case OpenVDS::MetadataType::DoubleVector3:
    return "DoubleVector3";
  case OpenVDS::MetadataType::DoubleVector4:
    return "DoubleVector4";
  case OpenVDS::MetadataType::String:
    return "String";
  case OpenVDS::MetadataType::BLOB:
    return "BLOB";
  }
  return "";
}

template<typename T, size_t N>
Json::Value getJsonFromVector(const OpenVDS::Vector<T, N> &vec)
{
  Json::Value ret;
  for (int i = 0; i < int(N); i++)
  {
    ret.append(vec[i]);
  }
  return ret;
}

Json::Value getJsonFromMetadata(const OpenVDS::MetadataKey &key, OpenVDS::VolumeDataLayout *layout)
{
  Json::Value value;
  value["category"] = key.GetCategory();
  value["name"] = key.GetName();
  value["type"] = MetadataTypeToString(key.GetType());
  switch (key.GetType())
  {
  case OpenVDS::MetadataType::Int:
    value["value"] = layout->GetMetadataInt(key.GetCategory(), key.GetName());
    break;
  case OpenVDS::MetadataType::IntVector2:
    value["value"] = getJsonFromVector(layout->GetMetadataIntVector2(key.GetCategory(), key.GetName()));
    break;
  case OpenVDS::MetadataType::IntVector3:
    value["value"] = getJsonFromVector(layout->GetMetadataIntVector3(key.GetCategory(), key.GetName()));
    break;
  case OpenVDS::MetadataType::IntVector4:
    value["value"] = getJsonFromVector(layout->GetMetadataIntVector4(key.GetCategory(), key.GetName()));
    break;
  case OpenVDS::MetadataType::Float:
    value["value"] = layout->GetMetadataFloat(key.GetCategory(), key.GetName());
    break;
  case OpenVDS::MetadataType::FloatVector2:
    value["value"] = getJsonFromVector(layout->GetMetadataFloatVector2(key.GetCategory(), key.GetName()));
    break;
  case OpenVDS::MetadataType::FloatVector3:
    value["value"] = getJsonFromVector(layout->GetMetadataFloatVector3(key.GetCategory(), key.GetName()));
    break;
  case OpenVDS::MetadataType::FloatVector4:
    value["value"] = getJsonFromVector(layout->GetMetadataFloatVector4(key.GetCategory(), key.GetName()));
    break;
  case OpenVDS::MetadataType::Double:
    value["value"] = layout->GetMetadataDouble(key.GetCategory(), key.GetName());
    break;
  case OpenVDS::MetadataType::DoubleVector2:
    value["value"] = getJsonFromVector(layout->GetMetadataDoubleVector2(key.GetCategory(), key.GetName()));
    break;
  case OpenVDS::MetadataType::DoubleVector3:
    value["value"] = getJsonFromVector(layout->GetMetadataDoubleVector3(key.GetCategory(), key.GetName()));
    break;
  case OpenVDS::MetadataType::DoubleVector4:
    value["value"] = getJsonFromVector(layout->GetMetadataDoubleVector4(key.GetCategory(), key.GetName()));
    break;
  case OpenVDS::MetadataType::String:
    value["value"] = layout->GetMetadataString(key.GetCategory(), key.GetName());
    break;
  case OpenVDS::MetadataType::BLOB:
    std::vector<uint8_t> blob;
    layout->GetMetadataBLOB(key.GetCategory(), key.GetName(), blob);
    std::vector<char> base64;
    OpenVDS::Base64Encode(blob.data(), int64_t(blob.size()), base64);
    std::string strbase64(base64.data(), base64.data() + base64.size());
    value["value"] = strbase64;
  }
  return value;
}

static std::string convertToString(const Json::Value &value)
{
  std::stringstream stream;
  Json::StreamWriterBuilder builder;
  builder["commentStyle"] = "None";
  builder["indentation"] = "   ";  // or whatever you like
  std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
  writer->write(value, &stream);
  return stream.str();
}

static bool autodetectDecode(const std::vector<uint8_t> &blob)
{
  int countEbcidicSpace = 0;
  int valuesGT127 = 0;
  for (auto c : blob)
  {
    if (c > 127)
      valuesGT127++;
    else if (c == '@')
      countEbcidicSpace++;
  }

  return countEbcidicSpace > blob.size() * 0.20 && valuesGT127 > blob.size() * 0.3;
}

static void decodedEbcdic(std::vector<uint8_t> &ebcdic)
{
  std::transform(ebcdic.begin(), ebcdic.end(), ebcdic.begin(), [](const uint8_t &d) { return ebcdic_to_ascii[d]; });
}

int main(int argc, char **argv)
{
  cxxopts::Options options("VDSInfo", "VDSInfo - A tool for extracting info from a VDS\n\nSee online documentation for connection paramters:\nhttp://osdu.pages.community.opengroup.org/platform/domain-data-mgmt-services/seismic/open-vds/connection.html\n");
  options.positional_help("<url>");

  std::vector<std::string> urlarg;
  std::string connection;
  std::string metadataPrintName;
  std::string metadataPrintCategory;

  bool axisDescriptors = false;
  bool channelDescriptors = false;
  bool volumeDataLayout = false;
  bool metaKeys = false;
  bool metadataFirstBlob = false;
  bool metadataAutoDecodeEBCDIC = false;
  bool metadataAll = false;
  int  textDecodeWidth = std::numeric_limits<int>::max();
  bool jsonOutput = false;
  bool help = false;
  bool version = false;

//connection options
  options.add_option("", "", "urlpos", "Url with vendor specific protocol.", cxxopts::value<std::vector<std::string>>(urlarg), "<string>");
  options.add_option("", "", "connection", "Vendor specific connection string.", cxxopts::value<std::string>(connection), "<string>");

///action
  options.add_option("", "", "axis", "Print axis descriptors.", cxxopts::value<bool>(axisDescriptors), "");
  options.add_option("", "", "channels", "Print channel descriptors.", cxxopts::value<bool>(channelDescriptors), "");
  options.add_option("", "", "layout", "Print layout.", cxxopts::value<bool>(volumeDataLayout), "");

  options.add_option("", "", "metadata-all", "Print all of the metadata", cxxopts::value<bool>(metadataAll), "");
  options.add_option("", "", "metadatakeys", "Print metadata keys.", cxxopts::value<bool>(metaKeys), "");
  options.add_option("", "", "metadata-name", "Print metadata matching name.", cxxopts::value<std::string>(metadataPrintName), "<string>");
  options.add_option("", "", "metadata-category", "Print metadata matching category.", cxxopts::value<std::string>(metadataPrintCategory), "<string>");
  options.add_option("", "b", "metadata-firstblob", "Print first blob found.", cxxopts::value<bool>(metadataFirstBlob), "");
  options.add_option("", "e", "metadata-autodecode", "Autodetect EBCDIC and decode to ASCII for blobs.", cxxopts::value<bool>(metadataAutoDecodeEBCDIC), "");
  options.add_option("", "w", "metadata-force-width", "Force output width.", cxxopts::value<int>(textDecodeWidth), "");
  
  options.add_option("", "", "url", "Url with vendor specific protocol or VDS file. (Available as positional argument as well).", cxxopts::value<std::vector<std::string>>(urlarg), "<string>");

  options.add_option("", "", "json-output", "Enable json output.", cxxopts::value<bool>(jsonOutput), "");
  options.add_option("", "h", "help", "Print this help information", cxxopts::value<bool>(help), "");
  options.add_option("", "", "version", "Print version information.", cxxopts::value<bool>(version), "");

  options.parse_positional("urlpos");

  if(argc == 1)
  {
    OpenVDS::printInfo(jsonOutput, "Args", options.help());
    return EXIT_SUCCESS;
  }

  try
  {
    options.parse(argc, argv);
  }
  catch(cxxopts::OptionParseException &e)
  {
    OpenVDS::printError(jsonOutput, "Args", e.what());
    return EXIT_FAILURE;
  }

  if(help)
  {
    OpenVDS::printInfo(jsonOutput, "Args", options.help());
    return EXIT_SUCCESS;
  }

  if (version)
  {
    OpenVDS::printVersion(jsonOutput, "VDSInfo");
    return EXIT_SUCCESS;
  }

  if (urlarg.empty())
  {
    OpenVDS::printError(jsonOutput, "Args", "Failed - missing url/vdsfile argument");
    return EXIT_FAILURE;
  }
  
  std::string url;
  if(!urlarg.empty()) url = urlarg[0];

  if (!axisDescriptors && !channelDescriptors && !volumeDataLayout && !metaKeys && metadataPrintName.empty() && metadataPrintCategory.empty() && !metadataAll)
  {
    axisDescriptors = true;
    channelDescriptors = true;
    volumeDataLayout = true;
    metadataAll = true;
  }

  OpenVDS::Error openError;

  OpenVDS::VDSHandle handle;

  if(OpenVDS::IsSupportedProtocol(url))
  {
    handle = OpenVDS::Open(url, connection, openError);
  }
  else
  {
    handle = OpenVDS::Open(OpenVDS::VDSFileOpenOptions(url), openError);
  }

  if(openError.code != 0)
  {
    OpenVDS::printError(jsonOutput, "VDS", "Could not open VDS", openError.string);
    return EXIT_FAILURE;
  }

  // auto-close vds handle when it goes out of scope
  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> vdsGuard(handle, &OpenVDS::Close);

  auto layout = OpenVDS::GetLayout(handle);
  if (!layout)
  {
    OpenVDS::printError(jsonOutput, "VDS", "Internal error, no layout");
    return EXIT_FAILURE;
  }
  
  Json::Value root;

  Json::Value layoutJson;
  if (volumeDataLayout)
  {
    layoutJson["layoutDescriptor"] = OpenVDS::SerializeVolumeDataLayoutDescriptor(*layout);
  }
  if (axisDescriptors)
  {
    layoutJson["axisDescriptors"] = OpenVDS::SerializeAxisDescriptors(*layout);
  }
  if (channelDescriptors)
  {
    layoutJson["channelDescriptors"] = OpenVDS::SerializeChannelDescriptors(*layout);
  }

  if (layoutJson.size())
    root["layoutInfo"] = layoutJson;

  if (metaKeys)
  {
    Json::Value metaKeysInfo;
    for (auto &key : layout->GetMetadataKeys())
    {
      Json::Value jsonKey;
      jsonKey["type"] = MetadataTypeToString(key.GetType());
      jsonKey["category"] = key.GetCategory();
      jsonKey["name"] = key.GetName();
      metaKeysInfo.append(jsonKey);
    }
    root["metaKeysInfo"] = metaKeysInfo;
  }

  if (metadataPrintName.size() || metadataPrintCategory.size() || metadataAll)
  {
    std::vector<OpenVDS::MetadataKey> to_print;
    std::vector<OpenVDS::MetadataKey> to_print_blobs;
    for (auto &key : layout->GetMetadataKeys())
    {
      if (!metadataAll)
      {
        if (metadataPrintName.size() && metadataPrintName != key.GetName())
          continue;
        if (metadataPrintCategory.size() && metadataPrintCategory != key.GetCategory() && !metadataAll)
          continue;
      }
      if (key.GetType() == OpenVDS::MetadataType::BLOB && metadataFirstBlob && !metadataAll)
        to_print_blobs.push_back(key);
      else
        to_print.push_back(key);
    }

    if (root.empty() && (metadataFirstBlob || to_print.empty()))
    {
      if (to_print_blobs.size())
      {
        auto &key = to_print_blobs.front();
        std::vector<uint8_t> vector;
        layout->GetMetadataBLOB(key.GetCategory(), key.GetName(), vector);
        bool decodeEBCDIC = false;
        if (metadataAutoDecodeEBCDIC)
        {
          decodeEBCDIC = autodetectDecode(vector);
        }
        if (decodeEBCDIC)
        {
          decodedEbcdic(vector);
        }
        int i = 0;
        while(i < int(vector.size()))
        {
          int to_copy = std::min(textDecodeWidth, int(vector.size() - i));
          fwrite(vector.data() + i, 1, to_copy, stdout);
          fwrite("\n", 1, 1, stdout);
          i += to_copy;
        }
      }
    }
    else
    {
      Json::Value metaInfo;
      for (auto& metadatakey : to_print)
      {
        metaInfo.append(getJsonFromMetadata(metadatakey, layout));
      }
      if (metaInfo.size())
        root["metadata"] = metaInfo;
    }
  }


  while (root.size() == 1)
    root = *root.begin();

  if (root.size())
  {
    std::string outstring = convertToString(root);
    fwrite(outstring.c_str(), 1, outstring.size(), stdout);
  }

  return EXIT_SUCCESS;
}
