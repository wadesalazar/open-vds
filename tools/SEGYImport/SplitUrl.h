#ifndef SPLIT_URL_H
#define SPLIT_URL_H

#include <string>
#include <OpenVDS/OpenVDS.h>
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

#endif