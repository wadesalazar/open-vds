#ifndef PRINT_HELPERS_H
#define PRINT_HELPERS_H

#include <json/json.h>
#include <fmt/format.h>

namespace OpenVDS
{
void
printInfo(bool jsonOutput, const std::string title, const std::string &str)
{
  if (jsonOutput)
  {
    Json::Value valueObj;
    valueObj["message"] = str;
    valueObj["title"] = title;
    Json::Value info;
    info["info"] = valueObj;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "  ";
    std::string document = Json::writeString(wbuilder, info);
    fmt::print(stdout, "{}", document);
  }
  else
  {
    fmt::print(stdout, "{}", str);
  }
}

void
printInfo(bool jsonOutput, const std::string title, const std::string &str, const std::string &value)
{
  if (jsonOutput)
  {
    Json::Value valueObj;
    valueObj["value"] = value;
    valueObj["message"] = str;
    valueObj["title"] = title;
    Json::Value info;
    info["info"] = valueObj;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "  ";
    std::string document = Json::writeString(wbuilder, info);
    fmt::print(stdout, "{}\n", document);
  }
  else
  {
    fmt::print(stdout, "{}: {}\n", str, value);
  }
}

void
printVersion(bool jsonOutput, const std::string &name)
{
  if (jsonOutput)
  {
    Json::Value version;
    version["name"] = name;
    version["project"] = PROJECT_NAME;
    version["version"] = PROJECT_VERSION;
    Json::Value info;
    info["version"] = version;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "  ";
    std::string document = Json::writeString(wbuilder, info);
    fmt::print(stdout, "{}\n", document);
  }
  else
  {
    fmt::print(stdout, "{} - {} {}\n", name, PROJECT_NAME, PROJECT_VERSION);
  }
}

void
printWarning(bool jsonOutput, const std::string &title, const std::string& str)
{
  if (jsonOutput)
  {
    Json::Value valueObj;
    valueObj["message"] = str;
    valueObj["title"] = title;
    Json::Value warning;
    warning["warning"] = valueObj;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "  ";
    std::string document = Json::writeString(wbuilder, warning);
    fmt::print(stdout, "{}\n", document);
  }
  else
  {
    fmt::print(stderr, "[{}]\n", str);
  }
}

void
printWarning(bool jsonOutput, const std::string& title, const std::string& message, const std::string& value, const std::string &systemError)
{
  if (jsonOutput)
  {
    Json::Value valueObj;
    valueObj["message"] = message;
    valueObj["title"] = title;
    valueObj["value"] = value;
    valueObj["error"] = systemError;
    Json::Value warning;
    warning["warning"] = valueObj;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "  ";
    std::string document = Json::writeString(wbuilder, warning);
    fmt::print(stdout, "{}\n", document);
  }
  else
  {
    fmt::print(stderr, "[{}] {}: {}\n", message, value, systemError);
  }
}

void
printError(bool jsonOutput, const std::string &title, const std::string& str)
{
  if (jsonOutput)
  {
    Json::Value valueObj;
    valueObj["message"] = str;
    valueObj["title"] = title;
    Json::Value error;
    error["error"] = valueObj;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "  ";
    std::string document = Json::writeString(wbuilder, error);
    fmt::print(stdout, "{}\n", document);
  }
  else
  {
    fmt::print(stderr, "[{}]\n", str);
  }
}

void
printError(bool jsonOutput, const std::string& title, const std::string& message, const std::string& value)
{
  if (jsonOutput)
  {
    Json::Value valueObj;
    valueObj["message"] = message;
    valueObj["title"] = title;
    valueObj["value"] = value;
    Json::Value error;
    error["error"] = valueObj;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "  ";
    std::string document = Json::writeString(wbuilder, error);
    fmt::print(stdout, "{}\n", document);
  }
  else
  {
    fmt::print(stderr, "[{}] {}\n", message, value);
  }
}

void
printError(bool jsonOutput, const std::string& title, const std::string& message, const std::string& value, const std::string &systemError)
{
  if (jsonOutput)
  {
    Json::Value valueObj;
    valueObj["message"] = message;
    valueObj["title"] = title;
    valueObj["value"] = value;
    valueObj["error"] = systemError;
    Json::Value error;
    error["error"] = valueObj;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "  ";
    std::string document = Json::writeString(wbuilder, error);
    fmt::print(stdout, "{}\n", document);
  }
  else
  {
    fmt::print(stderr, "[{}] {}: {}\n", message, value, systemError);
  }
}

void
printWarning_with_condition_fatal(bool jsonOutput, bool fatal, const std::string title, const std::string& value, const std::string& fatal_value)
{
  if (jsonOutput)
  {
    Json::Value valueObj;
    valueObj["message"] = value;
    valueObj["title"] = title;
    if (fatal)
      valueObj["info"] = fatal_value;
    Json::Value root;
    if (fatal)
      root["error"] = valueObj;
    else
      root["warning"] = valueObj;
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "  ";
    std::string document = Json::writeString(wbuilder, root);
    fmt::print(stdout, "{}\n", document);
  }
  else
  {
    printWarning(jsonOutput, title, value);
    if(fatal)
    {
      printError(jsonOutput, title, fatal_value);
    }
  }
  if (fatal)
    exit(1);
}

struct PrintWarningContext
{
  Json::Value arrayAcc;
  std::string title;
  std::string fatalMsg;
  bool jsonOutput;
  bool fatal;
  PrintWarningContext(bool jsonOutput, const std::string& title, bool fatal, const std::string fatalMsg)
    : title(title)
    , jsonOutput(jsonOutput)
    , fatal(fatal)
  {

  }
  ~PrintWarningContext()
  {
    if (jsonOutput)
    {
      Json::Value root;
      if (fatal)
      {
        root["error"] = arrayAcc;
        root["info"] = fatalMsg;
      }
      else
      {
        root["warning"] = arrayAcc;
      }
      Json::StreamWriterBuilder wbuilder;
      wbuilder["indentation"] = "  ";
      std::string document = Json::writeString(wbuilder, root);
      fmt::print(stdout, "{}\n", document);
    }
    else
    {
      if (fatal)
        printError(jsonOutput, "VDS", fatalMsg);
    }
    if (fatal)
    {
      exit(EXIT_FAILURE);
    }
  }

  void addWarning(const std::string& message, const std::string& value, const std::string& systemError)
  {
    if (jsonOutput)
    {
      Json::Value obj;
      obj["message"] = message;
      obj["title"] = title;
      obj["value"] = value;
      obj["error"] = systemError;
      arrayAcc.append(obj);
    }
    else
    {
      fmt::print(stderr, "[{}] {}: {}\n", message, value, systemError);
    }
  }
};

}

#endif
