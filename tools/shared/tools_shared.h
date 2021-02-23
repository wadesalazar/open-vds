#ifndef TOOLS_SHARED_H
#define TOOLS_SHARED_H

#include <string>
#include <OpenVDS/OpenVDS.h>
#include <SEGYUtils/DataProvider.h>
#include <fmt/format.h>

void splitUrlOnParameters(const std::string& full_url, std::string& url, std::string& parameters)
{
  auto parameters_start = std::find(full_url.begin(), full_url.end(), '?');

  url.insert(url.end(), full_url.begin(), parameters_start);
  parameters.insert(parameters.end(), parameters_start, full_url.end());
}

void splitUrl(const std::string& url, std::string &dirName, std::string &baseName, std::string &parameters, OpenVDS::Error& error)
{
  std::string url_no_param;
  splitUrlOnParameters(url, url_no_param, parameters);
  if (url_no_param.back() == '/')
  {
    error.code = -1;
    error.string = fmt::format("Missing file argument for url {}.", url);
    return;
  }
  auto last_slash = std::find(url_no_param.rbegin(), url_no_param.rend(), '/').base();
  dirName.insert(dirName.end(), url_no_param.begin(), last_slash - 1);
  baseName.insert(baseName.end(), last_slash, url_no_param.end());
}

DataProvider CreateDataProviderFromFile(const std::string &filename, OpenVDS::Error &error)
{
  std::unique_ptr<OpenVDS::File> file(new OpenVDS::File());
  if (!file->Open(filename, false, false, false, error))
    return DataProvider((OpenVDS::File *)nullptr);
  return DataProvider(file.release());
}

DataProvider CreateDataProviderFromOpenOptions(const std::string &url, const std::string &connectionString, const std::string &objectId, OpenVDS::Error &error)
{
  std::unique_ptr<OpenVDS::IOManager> ioManager(OpenVDS::IOManager::CreateIOManager(url, connectionString, OpenVDS::IOManager::AccessPattern::ReadOnly, error));
  if (error.code)
    return DataProvider((OpenVDS::IOManager *)nullptr, "", error);
  return DataProvider(ioManager.release(), objectId, error);
}

DataProvider CreateDataProvicerFromArgs(const std::string& name, const std::string& connection, OpenVDS::Error& error)
{
  if (OpenVDS::IsSupportedProtocol(name))
  {
    std::string dirname;
    std::string basename;
    std::string parameters;
    splitUrl(name, dirname, basename, parameters, error);
    if (error.code)
      return DataProvider(nullptr);

    std::string url = dirname + parameters;
    return CreateDataProviderFromOpenOptions(url, connection, basename, error);
  }
  else
  {
    return CreateDataProviderFromFile(name, error);
  }
  return DataProvider(nullptr);
}

std::vector<DataProvider> CreateDataProviders(const std::vector<std::string> &fileNames, const std::string &connection, OpenVDS::Error &error)
{
  std::vector<DataProvider>
    dataProviders;

  for (const auto& fileName : fileNames)
  {
    error = OpenVDS::Error();
    dataProviders.push_back(CreateDataProvicerFromArgs(fileName, connection, error));

    if (error.code != 0)
    {
      dataProviders.clear();
      break;
    }
  }
  return dataProviders;
}
#endif