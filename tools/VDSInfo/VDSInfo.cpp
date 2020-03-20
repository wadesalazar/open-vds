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

  bool axis_descriptors = false;
  bool channel_descriptors = false;
  bool volume_data_layout = false;
  bool SEGYText = false;
  bool SEGYTextEbcdicDecode = false;
  int  SEGYTextDecodedWidth = 80;

//connection options
  options.add_option("", "", "bucket", "AWS S3 bucket to connect to.", cxxopts::value<std::string>(bucket), "<string>");
  options.add_option("", "", "region", "AWS region of bucket to connect to.", cxxopts::value<std::string>(region), "<string>");
  options.add_option("", "", "connection-string", "Azure Blob Storage connection string.", cxxopts::value<std::string>(connectionString), "<string>");
  options.add_option("", "", "container", "Azure Blob Storage container to connect to.", cxxopts::value<std::string>(container), "<string>");
  options.add_option("", "", "parallelism-factor", "Azure parallelism factor.", cxxopts::value<int>(azureParallelismFactor), "<value>");
  options.add_option("", "", "prefix", "Top-level prefix to prepend to all object-keys.", cxxopts::value<std::string>(prefix), "<string>");
  options.add_option("", "", "persistentID", "persistentID", cxxopts::value<std::string>(persistentID), "<ID>");

///action
  options.add_option("", "", "axis", "Print axis descriptors", cxxopts::value<bool>(axis_descriptors), "");
  options.add_option("", "", "channels", "Print channel descriptors", cxxopts::value<bool>(channel_descriptors), "");
  options.add_option("", "", "layout", "Print layout", cxxopts::value<bool>(volume_data_layout), "");

  options.add_option("", "", "SEGYText", "Print SEGY Text header", cxxopts::value<bool>(SEGYText), "");
  options.add_option("", "", "SEGYTextDecode", "Assume EBCIDIC encoding in SEGY Text header and decode", cxxopts::value<bool>(SEGYTextEbcdicDecode), "");
  options.add_option("", "", "SEGYDecodedWidth", "When text decoding header, force linebreaks at width", cxxopts::value<int>(SEGYTextDecodedWidth), "<value>");

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

  Json::Value root;

  auto layout = OpenVDS::GetLayout(handle.get());
  if (!layout)
  {
    fmt::print(stderr, "Internal error, no layout\n");
    return EXIT_FAILURE;
  }
  if (volume_data_layout)
  {
    root["layoutDescriptor"] = OpenVDS::Internal::SerializeVolumeDataLayoutDescriptor(*layout);
  }
  if (axis_descriptors)
  {
    root["axisDescriptors"] = OpenVDS::Internal::SerializeAxisDescriptors(*layout);
  }
  if (channel_descriptors)
  {
    root["channelDescriptors"] = OpenVDS::Internal::SerializeChannelDescriptors(*layout);
  }

  if (root.size() == 1)
  {
    fmt::print(stdout, "{}\n", Json::StyledWriter().write(*root.begin()));
    return EXIT_SUCCESS;
  }
  else if (root.size() > 1)
  {
    fmt::print(stdout, "{}\n", Json::StyledWriter().write(root));
    return EXIT_SUCCESS;
  }

  if (SEGYText)
  {
    std::vector<uint8_t> header;
    layout->GetMetadataBLOB("SEGY", "TextHeader", header);
    if (SEGYTextEbcdicDecode)
    {
      std::string textheader;
      textheader.reserve(header.size() * 2);
      int lines = header.size() / SEGYTextDecodedWidth + 1;
      for (int line = 0; line < lines; line++)
      {
        int characters_on_line = std::min(int(header.size() - line * SEGYTextDecodedWidth), SEGYTextDecodedWidth);
        for (int i = 0; i < characters_on_line; i++)
        {
          int character = line * SEGYTextDecodedWidth + i;
          textheader.push_back(ebcdic_to_ascii[header[character]]);
        }
        textheader.push_back('\n');
      }
      fmt::print(stdout, "{}\n", textheader);
    }
    else
    {
      fwrite(header.data(), 1, header.size(), stdout);
    }
    EXIT_SUCCESS;
  }

  return EXIT_SUCCESS;
}
