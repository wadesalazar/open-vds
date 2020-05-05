#define _CRT_SECURE_NO_WARNINGS 1
#include "Env.h"

#include <stdlib.h>
#include <string.h>

namespace OpenVDS
{
bool getBooleanEnvironmentVariable(const char *name)
{
  const char *var = getenv(name);
  if (!var)
    return false;
  if (strcmp(var, "") == 0)
    return false;
  if (strcmp(var, "false") == 0)
    return false;
  if (strcmp(var, "FALSE") == 0)
    return false;
  if (strcmp(var, "0") == 0)
    return false;
  return true;
}
}
