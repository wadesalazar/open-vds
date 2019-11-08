# This script creates class files (.cpp/.h) with a given name.
# Base class and namespace can also be defined.
# Stein Pedersen 13.10.2019

from __future__ import print_function

import sys
import os.path
import runpy

header_template = """#ifndef CLASSNAME_H_INCLUDED
#define CLASSNAME_H_INCLUDED
Copyright

NAMESPACE_BEGIN

class ClassNameBASECLASS
{
public:
          ClassName();
  virtual ~ClassName();
};

NAMESPACE_END

#endif
"""

impl_template = """Copyright

#include "HeaderName"

NAMESPACE_BEGIN

ClassName::ClassName()
{
}

ClassName::~ClassName()
{
}

NAMESPACE_END

"""

usage_string = """Usage: make_class <-do> [ClassName] <variable=VariableName> ...
-d           : print debugging info
-o           : overwrite existing files
-i<filename> : include template
where variable is one of:"""

def usage(vars):
  print(usage_string)
  for key in vars.keys():
    print("{0:20}: {1:40} (default) -> '{2}'".format(key, "'{}'".format(vars[key]), substituteVar(key, vars)))

def printVars(vars):
  for key in vars.keys():
    print("{0:20}: {1:40} -> '{2}'".format(key, "'{}'".format(vars[key]), substituteVar(key, vars)))

def parseVar(value, tbegin, tend):
  token  = value[value.index(tbegin)+1:value.index(tend)]
  prefix = value[0:value.index(tbegin)]
  suffix = value[value.index(tend)+1:]
  return prefix, token, suffix

def substituteVars(value, vars):
    if value.startswith('?'):
      # For ?var|suffix, if vars[var] is non-empty, 
      # substitute ?var| with suffix, otherwise substitute it with ''
      pre, tok, suf = parseVar(value, '?', '|')
      if vars[tok]:
        value = substituteVars(suf, vars)
      else:
        value = ''
    elif '<' in value:
      # Substitute <var> with vars[var]
      pre, tok, suf = parseVar(value, '<', '>')
      value = pre + vars[tok] + suf
    elif '[' in value:
      # Substitute <var> with vars[var].upper()
      pre, tok, suf = parseVar(value, '[', ']')
      value = pre + vars[tok].upper() + suf
    return value
  
def substituteVar(key, vars):
    value = vars[key]
    return substituteVars(value, vars)

def substituteTemplate(vars, template):
  result = template
  for var in vars:
    result = result.replace(var, substituteVar(var, vars))
  return result

def trywritefile(fname, contents, overwrite):
  if os.path.isfile(fname) and not overwrite:
    print("File '{}' already exists. Use -o option to overwrite.".format(fname), file=sys.stderr)
  else:
    with open(fname, "w") as file:
      file.write(contents)
      
def main():
  debug = False
  overwrite = False
  vars = {
    "BaseName":         "",
    "Namespace":        "",
    "Copyright":        "",
    "ClassName":        "MyClass",
    "HeaderName":       "<ClassName>.h",
    "ImplName":         "<ClassName>.cpp",
    "CLASSNAME":        "[ClassName]",
    "NAMESPACE_BEGIN":  "?Namespace|namespace <Namespace> {",
    "NAMESPACE_END":    "?Namespace|}",
    "BASECLASS":        "?BaseName| : public <BaseName>"
  }
  if len(sys.argv) <= 1:
    usage(vars)
    exit(-1)
  for arg in sys.argv[1:]:
    if '=' in arg:
      key,val = arg.split('=')
      vars[key] = val
    elif arg.startswith('-d'):
      debug = True
    elif arg.startswith('-o'):
      overwrite = True
    elif arg.startswith('-i'):
      templatefile = arg[2:]
      d = runpy.run_path(templatefile)
      overridevars = [ k for k in d.keys() if k not in globals().keys() and not k.startswith('__')]
      for k in overridevars:
        vars[k] = d[k]
    else:
      vars['ClassName'] = arg
  if debug:
    printVars(vars)
  header = substituteTemplate(vars, header_template)
  headerfilename = substituteVar("HeaderName", vars)
  impl = substituteTemplate(vars, impl_template)
  implfilename = substituteVar("ImplName", vars)
  trywritefile(headerfilename, header, overwrite)
  trywritefile(implfilename, impl, overwrite)
  
if __name__ == "__main__":
  main()
