#include <fmt/printf.h>
#include <json/json.h>

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataLayout.h>

#include "cxxopts.hpp"

namespace OpenVDS
{
namespace Internal
{
  extern Json::Value SerializeVolumeDataLayoutDescriptor(VolumeDataLayout const &volumeDataLayout);
  extern Json::Value SerializeAxisDescriptors(VolumeDataLayout const &volumeDataLayout);
  extern Json::Value SerializeChannelDescriptors(VolumeDataLayout const &volumeDataLayout);
}
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
  for (int i = 0; i < N; i++)
  {
    ret.append(vec[i]);
  }
  return ret;
}

Json::Value getJsonFromMetadata(const OpenVDS::MetadataKey &key, OpenVDS::VolumeDataLayout *layout)
{
  Json::Value value;
  value["category"] = key.category;
  value["name"] = key.name;
  value["type"] = MetadataTypeToString(key.type);
  switch (key.type)
  {
  case OpenVDS::MetadataType::Int:
    value["value"] = layout->GetMetadataInt(key.category, key.name);
    break;
  case OpenVDS::MetadataType::IntVector2:
    value["value"] = getJsonFromVector(layout->GetMetadataIntVector2(key.category, key.name));
    break;
  case OpenVDS::MetadataType::IntVector3:
    value["value"] = getJsonFromVector(layout->GetMetadataIntVector3(key.category, key.name));
    break;
  case OpenVDS::MetadataType::IntVector4:
    value["value"] = getJsonFromVector(layout->GetMetadataIntVector4(key.category, key.name));
    break;
  case OpenVDS::MetadataType::Float:
    value["value"] = layout->GetMetadataFloat(key.category, key.name);
    break;
  case OpenVDS::MetadataType::FloatVector2:
    value["value"] = getJsonFromVector(layout->GetMetadataFloatVector2(key.category, key.name));
    break;
  case OpenVDS::MetadataType::FloatVector3:
    value["value"] = getJsonFromVector(layout->GetMetadataFloatVector3(key.category, key.name));
    break;
  case OpenVDS::MetadataType::FloatVector4:
    value["value"] = getJsonFromVector(layout->GetMetadataFloatVector4(key.category, key.name));
    break;
  case OpenVDS::MetadataType::Double:
    value["value"] = layout->GetMetadataDouble(key.category, key.name);
    break;
  case OpenVDS::MetadataType::DoubleVector2:
    value["value"] = getJsonFromVector(layout->GetMetadataDoubleVector2(key.category, key.name));
    break;
  case OpenVDS::MetadataType::DoubleVector3:
    value["value"] = getJsonFromVector(layout->GetMetadataDoubleVector3(key.category, key.name));
    break;
  case OpenVDS::MetadataType::DoubleVector4:
    value["value"] = getJsonFromVector(layout->GetMetadataDoubleVector4(key.category, key.name));
    break;
  case OpenVDS::MetadataType::String:
    value["value"] = layout->GetMetadataString(key.category, key.name);
    break;
  default:
    break;
  }
  return value;
}

static std::string convertToString(const Json::Value &value)
{
  std::stringstream stream;
  Json::StyledStreamWriter("  ").write(stream, value);
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
  cxxopts::Options options("VDSInfo", "VDSInfo - A tool for extracting info from a VDS");

  std::string bucket;
  std::string region;
  std::string connectionString;
  std::string container;
  int azureParallelismFactor = 0;
  std::string prefix;
  std::string persistentID;
  std::string metadataPrintName;
  std::string metadataPrintCategory;

  bool axisDescriptors = false;
  bool channelDescriptors = false;
  bool volumeDataLayout = false;
  bool metaKeys = false;
  bool metaDataFirstBlob = false;
  bool metadataAutoDecodeEPCIDIC = false;
  int  textDecodeWidth = std::numeric_limits<int>::max();

//connection options
  options.add_option("", "", "bucket", "AWS S3 bucket to connect to.", cxxopts::value<std::string>(bucket), "<string>");
  options.add_option("", "", "region", "AWS region of bucket to connect to.", cxxopts::value<std::string>(region), "<string>");
  options.add_option("", "", "connection-string", "Azure Blob Storage connection string.", cxxopts::value<std::string>(connectionString), "<string>");
  options.add_option("", "", "container", "Azure Blob Storage container to connect to.", cxxopts::value<std::string>(container), "<string>");
  options.add_option("", "", "parallelism-factor", "Azure parallelism factor.", cxxopts::value<int>(azureParallelismFactor), "<value>");
  options.add_option("", "", "prefix", "Top-level prefix to prepend to all object-keys.", cxxopts::value<std::string>(prefix), "<string>");
  options.add_option("", "", "persistentID", "persistentID", cxxopts::value<std::string>(persistentID), "<ID>");

///action
  options.add_option("", "", "axis", "Print axis descriptors", cxxopts::value<bool>(axisDescriptors), "");
  options.add_option("", "", "channels", "Print channel descriptors", cxxopts::value<bool>(channelDescriptors), "");
  options.add_option("", "", "layout", "Print layout", cxxopts::value<bool>(volumeDataLayout), "");

  options.add_option("", "", "metadatakeys", "Print metadata keys", cxxopts::value<bool>(metaKeys), "");
  options.add_option("", "", "metadata-name", "Print metadata matching name", cxxopts::value<std::string>(metadataPrintName), "<string>");
  options.add_option("", "", "metadata-category", "Print metadata matching category", cxxopts::value<std::string>(metadataPrintCategory), "<string>");
  options.add_option("", "b", "metadata-firstblob", "Print first blob found", cxxopts::value<bool>(metaDataFirstBlob), "");
  options.add_option("", "e", "metadata-autodecode", "Autodetect EPCIDIC and decode to ASCII for blobs", cxxopts::value<bool>(metadataAutoDecodeEPCIDIC), "");
  options.add_option("", "w", "metadata-force-width", "Force output width", cxxopts::value<int>(textDecodeWidth), "");

  if(argc == 1)
  {
    std::cout << options.help();
    return EXIT_SUCCESS;
  }

  try
  {
    options.parse(argc, argv);
  }
  catch(cxxopts::OptionParseException e)
  {
    fmt::print(stderr, "{}", e.what());
    return EXIT_FAILURE;
  }

  // Open the VDS
  std::string key = !prefix.empty() ? prefix + "/" + persistentID : persistentID;

  std::unique_ptr<OpenVDS::OpenOptions> openOptions;

  if(!bucket.empty())
  {
    openOptions.reset(new OpenVDS::AWSOpenOptions(bucket, key, region));
  }
  else if(!container.empty())
  {
    openOptions.reset(new OpenVDS::AzureOpenOptions(connectionString, container, key));
  }

  if(azureParallelismFactor)
  {
    if(openOptions->connectionType == OpenVDS::OpenOptions::Azure)
    {
      auto &azureOpenOptions = *static_cast<OpenVDS::AzureOpenOptions *>(openOptions.get());

      azureOpenOptions.parallelism_factor = azureParallelismFactor;
    }
    else
    {
      std::cerr << "Cannot specify parallelism-factor with other backends than Azure\n";
      return EXIT_FAILURE;
    }
  }

  OpenVDS::Error openError;

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(*openOptions.get(), openError), &OpenVDS::Close);

  if(openError.code != 0)
  {
    fmt::print(stderr, "Could not open VDS: {}\n", openError.string);
    return EXIT_FAILURE;
  }

  auto layout = OpenVDS::GetLayout(handle.get());
  if (!layout)
  {
    fmt::print(stderr, "Internal error, no layout\n");
    return EXIT_FAILURE;
  }
  
  Json::Value root;

  Json::Value layoutJson;
  if (volumeDataLayout)
  {
    layoutJson["layoutDescriptor"] = OpenVDS::Internal::SerializeVolumeDataLayoutDescriptor(*layout);
  }
  if (axisDescriptors)
  {
    layoutJson["axisDescriptors"] = OpenVDS::Internal::SerializeAxisDescriptors(*layout);
  }
  if (channelDescriptors)
  {
    layoutJson["channelDescriptors"] = OpenVDS::Internal::SerializeChannelDescriptors(*layout);
  }

  if (layoutJson.size())
    root["layoutInfo"] = layoutJson;

  if (metaKeys)
  {
    Json::Value metaKeysInfo;
    auto keys = layout->GetMetadataKeys();
    for (auto &key : layout->GetMetadataKeys())
    {
      Json::Value jsonKey;
      jsonKey["type"] = MetadataTypeToString(key.type);
      jsonKey["category"] = key.category;
      jsonKey["name"] = key.name;
      metaKeysInfo.append(jsonKey);
    }
    root["metaKeysInfo"] = metaKeysInfo;
  }

  if (metadataPrintName.size() || metadataPrintCategory.size())
  {
    std::vector<OpenVDS::MetadataKey> to_print;
    std::vector<OpenVDS::MetadataKey> to_print_blobs;
    for (auto &key : layout->GetMetadataKeys())
    {
      if (metadataPrintName.size() && metadataPrintName != key.name)
        continue;
      if (metadataPrintCategory.size() && metadataPrintCategory != key.category)
        continue;
      if (key.type == OpenVDS::MetadataType::BLOB)
        to_print_blobs.push_back(key);
      else
        to_print.push_back(key);
    }

    if (root.empty() && (metaDataFirstBlob || to_print.empty()))
    {
      if (to_print_blobs.size())
      {
        auto &key = to_print_blobs.front();
        std::vector<uint8_t> vector;
        layout->GetMetadataBLOB(key.category, key.name, vector);
        bool decodeEPCIDIC = false;
        if (metadataAutoDecodeEPCIDIC)
        {
          decodeEPCIDIC = autodetectDecode(vector);
        }
        if (decodeEPCIDIC)
        {
          decodedEbcdic(vector);
        }
        int i = 0;
        while(i < vector.size())
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
