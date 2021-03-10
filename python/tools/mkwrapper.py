############################################################################
# Copyright 2019 The Open Group
# Copyright 2019 Bluware, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################/

import os
import sys
import platform
import re
import textwrap
import fnmatch
from os import path

from clang import cindex
from clang.cindex import CursorKind
from collections import OrderedDict
from glob import glob
from threading import Thread, Semaphore
from multiprocessing import cpu_count


make_trampolines_readable = False # If True, don't put everything on one line

#import cymbal
#import ctypes
#
#cymbal.monkeypatch_type('get_template_argument_type',
#                        'clang_Type_getTemplateArgumentAsType',
#                        [cindex.Type, ctypes.c_uint],
#                        cindex.Type)
#
#cymbal.monkeypatch_type('get_num_template_arguments',
#                        'clang_Type_getNumTemplateArguments',
#                        [cindex.Type],
#                        ctypes.c_int)
#
## check if the cursor's type is a template
#def is_template(node):
#    return hasattr(node, 'type') and node.type.get_num_template_arguments() != -1

RECURSE_LIST = [
    CursorKind.TRANSLATION_UNIT,
    CursorKind.NAMESPACE,
    CursorKind.CLASS_DECL,
    CursorKind.STRUCT_DECL,
    CursorKind.ENUM_DECL,
    CursorKind.CXX_METHOD,
    CursorKind.FUNCTION_DECL,
]

PRINT_LIST = [
    CursorKind.CLASS_DECL,
    CursorKind.STRUCT_DECL,
    CursorKind.ENUM_DECL,
    CursorKind.ENUM_CONSTANT_DECL,
    CursorKind.FUNCTION_DECL,
#    CursorKind.FUNCTION_TEMPLATE,
    CursorKind.CONVERSION_FUNCTION,
    CursorKind.CXX_METHOD,
    CursorKind.CXX_BASE_SPECIFIER,
    CursorKind.CONSTRUCTOR,
    CursorKind.DESTRUCTOR,
    CursorKind.FIELD_DECL,
    CursorKind.PARM_DECL,
    CursorKind.TYPE_ALIAS_DECL,
    CursorKind.CLASS_TEMPLATE,
]

PREFIX_BLACKLIST = [
    CursorKind.TRANSLATION_UNIT
]

PUBLIC_LIST = [
  cindex.AccessSpecifier.INVALID,
  cindex.AccessSpecifier.PUBLIC
]

PROTECTED_LIST = [
  cindex.AccessSpecifier.INVALID,
  cindex.AccessSpecifier.PUBLIC,
  cindex.AccessSpecifier.PROTECTED
]

PRIVATE_LIST = [
  cindex.AccessSpecifier.INVALID,
  cindex.AccessSpecifier.PUBLIC,
  cindex.AccessSpecifier.PROTECTED,
  cindex.AccessSpecifier.PRIVATE
]

_AUTOGEN_FAIL_LIST = []

CPP_OPERATORS = {
    '<=': 'le', '>=': 'ge', '==': 'eq', '!=': 'ne', '[]': 'array',
    '+=': 'iadd', '-=': 'isub', '*=': 'imul', '/=': 'idiv', '%=':
    'imod', '&=': 'iand', '|=': 'ior', '^=': 'ixor', '<<=': 'ilshift',
    '>>=': 'irshift', '++': 'inc', '--': 'dec', '<<': 'lshift', '>>':
    'rshift', '&&': 'land', '||': 'lor', '!': 'lnot', '~': 'bnot',
    '&': 'band', '|': 'bor', '+': 'add', '-': 'sub', '*': 'mul', '/':
    'div', '%': 'mod', '<': 'lt', '>': 'gt', '=': 'assign', '()': 'call'
}

CPP_OPERATORS = OrderedDict(
    sorted(CPP_OPERATORS.items(), key=lambda t: -len(t[0])))

def d(s):
    return s if isinstance(s, str) else s.decode('utf8')
    
def q(s):
    return '"' + s + '"'
    
def sanitize_name(name):
    name = re.sub(r'type-parameter-0-([0-9]+)', r'T\1', name)
    for k, v in CPP_OPERATORS.items():
        name = name.replace('operator%s' % k, 'operator_%s' % v)
    name = re.sub('<.*>', '', name)
    name = ''.join([ch if ch.isalnum() else '_' for ch in name])
    name = re.sub('_$', '', re.sub('_+', '_', name))
    return name

def extract_nodes(filename, node, output, prefix=''):
    if not (node.location.file is None or
            os.path.samefile(d(node.location.file.name), filename)):
        return 0
#    print("{}:{}".format(str(node.kind), node.spelling))
#    if node.kind == CursorKind.TYPE_ALIAS_DECL:
#        print(node.spelling)
    if node.kind in RECURSE_LIST:
        sub_prefix = prefix
        if node.kind not in PREFIX_BLACKLIST:
            if len(sub_prefix) > 0:
                sub_prefix += '_'
            sub_prefix += d(node.spelling)
        for i in node.get_children():
            extract_nodes(filename, i, output, sub_prefix)
    if node.kind in PRINT_LIST:
        output.append(node)
    return output

def dump_node(n, file=sys.stdout):
    print("name: {0:40} {2:20} displayname: {1}".format(n.spelling, n.displayname, str(n.kind).replace('CursorKind.', '')), file)

def getpyname(name):
    return name[0].lower() + name[1:] if not name[1:2].isupper() else name

def getparent(node, all_):
    if node.semantic_parent:
        return node.semantic_parent
    for n in all_:
        if node in n.get_children():
            return n
    return None
    
def getchildren(node, all_, kind=None, access_filter = PUBLIC_LIST):
    children = list(node.get_children())
    children = [x for x in children if (kind is None or x.kind == kind)]
    children = [x for x in children if x.access_specifier in access_filter]
    return children
    
def getbases(node, all_):
    return getchildren(node, all_, CursorKind.CXX_BASE_SPECIFIER)
    
def getoverloads(node, all_, access_filter = PUBLIC_LIST):
    parent = getparent(node, all_)
    children = all_ if not parent else getchildren(parent, all_, node.kind, access_filter)
    return [x for x in children if x.kind == node.kind and x.spelling == node.spelling]

def getscope(node, all_, output):
    p = getparent(node, all_)
    while p and not p.kind == CursorKind.NAMESPACE and not p.kind == CursorKind.TRANSLATION_UNIT:
        output.append(p)
        p = getparent(p, all_)
    output  .reverse()
    return output

def getfullname(node, all_):
    scope = getscope(node, all_, [])
    prefix = ''
    for s in scope:
        prefix += s.spelling + '::'
    return "{}{}".format(prefix, node.spelling)

def fixname(name):
    name = name.replace("OpenVDS::", "native::")
    return name

def getnativename(node, all_):
    name = fixname(getfullname(node, all_))
    return name

def find_node_updwards(node, all_, kind, name):
    if not node:
        return None
    children = getchildren(node, all_, kind)
    children = [c for c in children if c.spelling == name]
    if children:
        return children[0]
    else:
        return find_node_updwards(getparent(node, all_), all_, kind, name)

def fixarglist(arglist_, node, all_):
    params = get_args(node, all_)
    arglist = arglist_
    for arg in params:
        t,_,_ = arg
        if t.startswith('enum '):
            n = t.replace('enum ', '')
            decl = find_node_updwards(node, all_, CursorKind.ENUM_DECL, n)
            n = getfullname(decl, all_)
            arglist = arglist.replace(t, n)
    arglist = fixname(arglist)
    return arglist

def getvarname(node, all_):
    return getfullname(node, all_).replace('::', '_') + '_'
    
def getdestructor(node, all_):
    dtor = next(iter([x for x in all_ if x.kind == CursorKind.DESTRUCTOR and x.semantic_parent == node]), None)
    return dtor

def format_docstring_decl(fullname):
    return "OPENVDS_DOCSTRING({})".format(fullname.replace('::', '_'))
    
def resolve_overload_name(node, all_):
    overloads = getoverloads(node, all_)
    suffix = ''
    index = 0
    try:
        index = overloads.index(node)
    except:
        pass
    if index > 0:
        suffix += '_{}'.format(index + 1)
    return sanitize_name(getfullname(node, all_).replace('::', '_')) + suffix # sanitize_name is not really meant for this...
    
def generate_none(node, all_, output, indent, parent_prefix, context):
    pass

def generate_classtemplate(node, all_, output, indent, parent_prefix, context):
    context["class_templates"][node.spelling] = node

# For normal classes, node == classnode. For template specializations, 
# node is the specialization and classnode is the template class declaration
def format_class_decl(node, classnode, all_, indent, parent_prefix):
    varname = getvarname(node, all_)
    bases = getbases(classnode, all_)
    basesdecl = ''
    for b in bases:
        basesdecl += ', {}'.format(getnativename(b.get_definition(), all_))
    deletor = ''
    dtor = getdestructor(node, all_)
    if dtor and not dtor.access_specifier == cindex.AccessSpecifier.PUBLIC:
        deletor = ", std::unique_ptr<{}, py::nodelete>".format(getnativename(node, all_))
    elif getnativename(node, all_).startswith('VolumeDataRequest'):
        deletor = ", std::shared_ptr<{}>".format(getnativename(node, all_))
    code = [ 
        '',
        indent + """// {}""".format(
            getfullname(node, all_)
        ),
        indent + """py::class_<{}{}{}> \n{}  {}({},"{}", {});""".format(
            getnativename(node, all_),
            basesdecl,
            deletor,
            indent,
            varname,
            parent_prefix,
            getfullname(node, all_),
            format_docstring_decl(getfullname(classnode, all_))
        ),
        ''
    ]
    return code

# Generate wrapper for explicit template specialization
def generate_typealias(node, all_, output, indent, parent_prefix, context):
#    if 'Float' in node.spelling:
#        debug = 1
    canonical = node.type.get_canonical()
#    print("{} = {}".format(node.spelling, canonical.spelling))
    template_params = canonical.spelling.replace('<', ',').replace('>', ',').split(',')[1:-1]
    children = [*node.get_children()]
    if len(children) >= 1 and children[0].kind == CursorKind.TEMPLATE_REF and len(template_params) > 0:
        class_template = children[0]
        real_type_name = "{}<{}>".format(class_template.spelling, ", ".join(template_params))
        if class_template.spelling in context["class_templates"]:
            if real_type_name not in context["class_template_instantiations"]:
                context["class_template_instantiations"][real_type_name] = node
#                print("Alias: {} = {}".format(node.spelling, real_type_name))
                template_node = context["class_templates"][class_template.spelling]
                code = format_class_decl(node, template_node, all_, indent, parent_prefix)
                output.extend(code)
                output.append('')
            else:
                output.append(indent + "// Ignored: {} = {} already defined as {}".format(node.spelling, real_type_name, context["class_template_instantiations"][real_type_name].spelling))
                pass

def generate_field(node, all_, output, indent, parent_prefix, context):
    overload_name = resolve_overload_name(node, all_)
    code = """.def_readwrite({0:30}, &{1:30}, {2});""".format(
        q(getpyname(node.spelling)),
        getnativename(node, all_),
        format_docstring_decl(overload_name)
    )
    output.append(indent + parent_prefix + code)

literal_types = [ 
    CursorKind.CHARACTER_LITERAL, 
    CursorKind.COMPOUND_LITERAL_EXPR, 
    CursorKind.CXX_BOOL_LITERAL_EXPR, 
    CursorKind.CXX_NULL_PTR_LITERAL_EXPR, 
    CursorKind.FLOATING_LITERAL, 
    CursorKind.IMAGINARY_LITERAL, 
    CursorKind.INTEGER_LITERAL, 
    CursorKind.OBJC_STRING_LITERAL, 
    CursorKind.OBJ_BOOL_LITERAL_EXPR, 
    CursorKind.STRING_LITERAL 
]

def get_default_paramvalue(node):
    assert node.kind == CursorKind.PARM_DECL
    c = [*node.get_children()]
    if c and [item.kind for item in c if item.kind in literal_types]:
        assert len(c) == 1, "fixme!"
        item = c[0]
        tokens = " ".join([token.spelling for token in item.get_tokens()])
#        print("{}: {} {}".format(node.spelling, str(item.kind), tokens))
        return tokens
    else:
        return None

def get_args(node, all_):
    paramnodes = getchildren(node, all_, CursorKind.PARM_DECL)
    params = [(n.type.spelling, n.spelling, get_default_paramvalue(n)) for n in paramnodes]
    return params

def get_argnames(node, all_):
    params = get_args(node, all_)
    argnames = ''
    if params:   
        for p in params:
            typ, name, defaultValue = p
            typ.strip()
            argnames += ', ' + format_parameter_decl(typ, name, defaultValue)
    return argnames

def generate_constructor(node, all_, output, indent, parent_prefix, context):
    overload_name = resolve_overload_name(node, all_)
    arglist = fixarglist(node.displayname[node.displayname.find('(') + 1:-1], node, all_)
    argnames = get_argnames(node, all_)
    code = """.def(py::init<{0:30}>(){1}, {2});""".format(
       arglist,
       argnames,
       format_docstring_decl(overload_name)
    )
    output.append(indent + parent_prefix + code)

def can_generate_function(restype, arglist):
#    if "cudaStream_t" in arglist:
#        return False
    if 'ProxyBLOB' in restype:
        return False
    if 'uint8_t *' in arglist: # This has room for improvement...
        return False
    if '[' in arglist or 'void *' in arglist or '**' in arglist:
        return False
    if 'IntVector' in arglist or 'FloatVector' in arglist or 'DoubleVector' in arglist:
        return False
    if not '<' in restype:
        if 'IntVector' in restype or 'FloatVector' in restype or 'DoubleVector' in restype:
            return False
    return True

class UnsupportedFunctionSignatureError(ValueError):
    pass

def get_adapter_type(native_type):
    if 'Vector' in native_type:
        tmp = native_type.replace("&", "").replace("const", "").strip()
        return re.sub(".+[:]+(.+)", r"\1Adapter", tmp)
    else:
        return native_type

buffer_types = [ 
    'void', 
    'uint8_t', 
    'uint16_t', 
    'uint32_t', 
    'uint64_t', 
    'int8_t', 
    'int16_t', 
    'int32_t', 
    'int64_t', 
    'float',
    'double'  
]

size_types = [
  'int',
  'size_t',
  'int64_t'
]

def format_parameter_decl(arg, argname, defaultValue):
    nullAllowed = False
    if 'optional' in arg:
        nullAllowed = True
        defaultValue = 'nullptr'
    initializer = ' = {}'.format(defaultValue) if defaultValue else ''
    argnone = '.none(false)' if not nullAllowed and not defaultValue else ''
    decl = 'py::arg("{}"){}{}'.format(argname, argnone, initializer)
    return decl

def try_generate_trampoline_function(node, all_, restype, arglist, params):
    args = arglist[1:-1].split(',')
    newargs = []
    callargs = []
    call_prefix = ''
    argnames = ''
    if 'ProxyBLOB' in restype:
         raise UnsupportedFunctionSignatureError(restype)
    if node.kind == CursorKind.CXX_METHOD:
        instance_arg = "{}* self".format(getnativename(getparent(node, all_), all_))
        newargs.append(instance_arg)
        call_prefix = 'self->'
    iParam = 0
    while iParam < len(params):
        p = params[iParam]
        arg, argname, defaultValue = p
        arg = arg.strip()
        nextarg, nextargname, nextdefaultvalue = params[iParam + 1] if iParam + 1 < len(params) else ('','',None)
        argnames += ', ' + format_parameter_decl(arg, argname, defaultValue)
        if '[' in arg:
            m=re.match(r"(.+)\((.+)\)\[(.+)\]", arg)
            if m:
                typ = m.groups()[0].replace('const', '').strip()
                cnt = int(m.groups()[2])
                ref = m.groups()[1]
                is_mutable = 'const' not in m.groups()[0]
                const = '' if is_mutable else 'const '
                if ref == '*' and is_mutable:
#                    print("HEPP! {}".format(arg))
                    raise UnsupportedFunctionSignatureError()
                else:
                    newarg = "{}py::array_t<{},{}>&".format(
                        const,
                        typ,
                        "py::array::c_style" if is_mutable else "py::array::forcecast"
                    )
                    array_function = 'getArrayPtrChecked' if ref == '*' else 'getArrayChecked'
                    callarg = "PyArrayAdapter<{}, {}, {}>::{}({})".format(
                        typ, 
                        cnt,
                        "true" if is_mutable else "false",
                        array_function,
                        argname)
                callargs.append(callarg)
                newargs.append("{} {}".format(newarg, argname))
            else:
                raise UnsupportedFunctionSignatureError(arglist)
            pass
        elif '**' in arg:
            raise UnsupportedFunctionSignatureError(arglist)
        elif '*' in arg and any(t in arg for t in buffer_types):
            is_mutable = False if 'const' in arg else True
            buffer_type = arg.replace('*', '').strip()
            newarg = "py::buffer"
            callarg = "PyGetBufferPtr<{}, {}>({})".format(buffer_type, 'true' if is_mutable else 'false', argname)
            callargs.append(callarg)
            newargs.append("{} {}".format(newarg, argname))
            if 'size' in nextargname.lower() and any(t in nextarg for t in size_types):
              callarg = "PyGetBufferSize<{}, {}>({})".format(nextarg, 'true' if is_mutable else 'false', argname)
              callargs.append(callarg)
              iParam += 1
        elif 'Vector' in arg:
            arg = "{}::AdaptedType".format(get_adapter_type(arg))
            callargs.append(argname)
            newargs.append("{} {}".format(arg, argname))
        else:
            callargs.append(argname)
            newargs.append("{} {}".format(arg, argname))
        iParam += 1
    call = "{}{}({})".format(call_prefix, node.spelling, ", ".join(callargs))        
    if 'Vector' in restype:
        restype = get_adapter_type(restype)
        call = "({}::AdaptedType)({})".format(restype, call)
    newarglist = ", ".join(newargs)
    sig = "[]({}) BEGIN return {}; END".format(newarglist, call)
    if make_trampolines_readable:
      sig = sig.replace('BEGIN', '\n    {\n      ').replace('END', '\n    }')
    else:
      sig = sig.replace('BEGIN', '{').replace('END', '}')
    sig += argnames
    return sig

relational_operators = {
    "operator_eq": ".def(py::self == py::self);",
    "operator_ne": ".def(py::self != py::self);",
}

def generate_function(node, all_, output, indent, parent_prefix, context):
#    if 'RequestSerializedVolumeDataChunk' in node.displayname:
#        a = 1000
    autogen_failed = False
    if node.get_num_template_arguments() >= 0:
        # Don't generate wrappers for template specializations
        return
    params = get_args(node, all_)
    argnames = get_argnames(node, all_)
    overload_name = resolve_overload_name(node, all_)
    fnname = getpyname(sanitize_name(node.spelling))
    if fnname in relational_operators.keys():
        code = relational_operators[fnname]
        line = indent + parent_prefix + code
        output.append(line)
        return
    restype   = fixname(node.result_type.spelling)
    arglist = fixarglist(fixname(node.displayname[node.displayname.find('('):]), node, all)
    method_suffix = ''
    method_prefix = ''
    if node.kind == CursorKind.CXX_METHOD:
        if not node.is_static_method():
            method_prefix = getnativename(node.semantic_parent, all_) + '::'
        if node.is_const_method():
            method_suffix = " const"
    code = """.def({0:30}, static_cast<{1}({2}*){3}{4}>(&{5}){7}, py::call_guard<py::gil_scoped_release>(), {6});""".format(
       q(fnname),
       restype,
       method_prefix,
       arglist,
       method_suffix,
       getnativename(node, all_),
       format_docstring_decl(overload_name),
       argnames
    )
    line = ''
    if not can_generate_function(restype, arglist):
        try:
            code = """.def({0:30}, {1}, py::call_guard<py::gil_scoped_release>(), {2});""".format(
               q(fnname),
               try_generate_trampoline_function(node, all_, restype, arglist, params),
               format_docstring_decl(overload_name)
            )
        except UnsupportedFunctionSignatureError:
            autogen_failed = True
            line = '// AUTOGENERATE FAIL : '
            _AUTOGEN_FAIL_LIST.append(node)
    if node.is_static_method():
        code = code.replace('.def(', '.def_static(')
    line = line + indent + parent_prefix + code
    output.append(line)
    if len(argnames) == 0 and (fnname.startswith("get") or fnname.startswith("is")) and not fnname.isdigit() and not fnname == "getGlobalState" and not fnname == "getLODLevels":
        if fnname.startswith("get"):
            pname = fnname[3:]
        else:
            pname = fnname[2:]
        pname = getpyname(pname)
#        print("Getter: {}".format(pname))
        getter_code = """.def_property_readonly("{}", &{}, {});""".format(
            pname,
            getnativename(node, all_),
            format_docstring_decl(overload_name)
        )
        getter_line = indent + parent_prefix + getter_code
        if not autogen_failed:
            output.append(getter_line)
    
def generate_class(node, all_, output, indent, parent_prefix, context):
    if node.is_definition():
        if '<' in node.displayname:
            _AUTOGEN_FAIL_LIST.append(node)
            return
        code = format_class_decl(node, node, all_, indent, parent_prefix)
        varname = getvarname(node, all_)
        for kind in CLASS_NODES:
            children = getchildren(node, all_, kind)
            for child in children:
                generate = NODE_HANDLERS[kind]
                generate(child, all_, code, indent, varname, context)
        output.extend(code)
        output.append('')
    else:
        pass

def generate_enumvalue(node, all_, output, indent, parent_prefix, context):
    code = [
        indent + """{0}.value({1:30}, {2:40}, {3});""".format(
            parent_prefix,
            q(node.spelling),
            getnativename(node, all_),
            format_docstring_decl(getfullname(node, all_))
        ),
    ]
    output.extend(code)
    
def generate_enum(node, all_, output, indent, parent_prefix, context):
    if node.get_definition() and node.spelling:
        varname = getvarname(node, all_)
        bases = getbases(node, all_)
        basesdecl = ''
        for b in bases:
            basesdecl += ', {}'.format(getnativename(b.get_definition(), all_))
        code = [ 
            '',
            indent + """py::enum_<{}> \n{}  {}({},"{}", {});""".format(
                getnativename(node, all_),
                indent,
                varname,
                parent_prefix,
                node.spelling,
                format_docstring_decl(getfullname(node, all_))
            ),
            ''
        ]
        for kind in ENUM_NODES:
            children = getchildren(node, all_, kind)
            for child in children:
                generate = NODE_HANDLERS[kind]
                generate(child, all_, code, indent, varname, context)
        output.extend(code)
        output.append('')
    else:
        pass
    
def generate_struct(*args):
    generate_class(*args)
    
NODE_HANDLERS = {
    CursorKind.STRUCT_DECL:         generate_struct,
    CursorKind.CLASS_DECL:          generate_class,
    CursorKind.ENUM_DECL:           generate_enum,
    CursorKind.ENUM_CONSTANT_DECL:  generate_enumvalue,
    CursorKind.FUNCTION_DECL:       generate_function,
    CursorKind.FIELD_DECL:          generate_field,
    CursorKind.CONSTRUCTOR:         generate_constructor,
    CursorKind.CXX_METHOD:          generate_function,
    CursorKind.TYPE_ALIAS_DECL:     generate_typealias,
    CursorKind.CLASS_TEMPLATE:      generate_classtemplate,
}

ENUM_NODES = [
    CursorKind.ENUM_CONSTANT_DECL,
]

CLASS_NODES = [
    CursorKind.CONSTRUCTOR,
    CursorKind.CXX_METHOD,
    CursorKind.FIELD_DECL,
    CursorKind.FUNCTION_DECL,
    CursorKind.CLASS_DECL,
    CursorKind.STRUCT_DECL,
    CursorKind.ENUM_DECL,
]

class Parser(object):
    def __init__(self, filename, parameters, output):
        self.filename   = filename
        self.parameters = parameters
        self.output     = output
        self.nodes      = []

    def run(self):
        print('Processing "%s" ..' % self.filename, file=sys.stderr)
        context = {
          "filename":                       self.filename,
          "parameters":                     self.parameters,
          "classes":                        {},
          "class_templates":                {},
          "class_template_instantiations":  {},
        }
        index = cindex.Index(cindex.conf.lib.clang_createIndex(False, True))
        tu = index.parse(self.filename, self.parameters)
        extract_nodes(self.filename, tu.cursor, self.nodes)
        for n in self.nodes:
#            dump_node(n)
            p = getparent(n, self.nodes)
            if p is None or p.kind == CursorKind.NAMESPACE or p.kind == CursorKind.TRANSLATION_UNIT:
                if n.kind in NODE_HANDLERS.keys():
                    func = NODE_HANDLERS[n.kind]
                    func(n, self.nodes, self.output, '  ', 'm', context)

def parse_args(args):
    parameters = []
    filenames = []
    if "-x" not in args:
        parameters.extend(['-x', 'c++'])
    if platform.system() == 'Darwin':
        dev_path = '/Applications/Xcode.app/Contents/Developer/'
        lib_dir = dev_path + 'Toolchains/XcodeDefault.xctoolchain/usr/lib/'
        sdk_dir = dev_path + 'Platforms/MacOSX.platform/Developer/SDKs'
        libclang = lib_dir + 'libclang.dylib'

        if os.path.exists(libclang):
            cindex.Config.set_library_path(os.path.dirname(libclang))

        if os.path.exists(sdk_dir):
            sysroot_dir = os.path.join(sdk_dir, next(os.walk(sdk_dir))[1][0])
            parameters.append('-isysroot')
            parameters.append(sysroot_dir)
    elif platform.system() == 'Linux':
        # clang doesn't find its own base includes by default on Linux,
        # but different distros install them in different paths.
        # Try to autodetect, preferring the highest numbered version.
        def clang_folder_version(d):
            return [int(ver) for ver in re.findall(r'(?<!lib)(?<!\d)\d+', d)]
        clang_include_dir = max((
            path
            for libdir in ['lib64', 'lib', 'lib32']
            for path in glob('/usr/%s/clang/*/include' % libdir)
            if os.path.isdir(path)
        ), default=None, key=clang_folder_version)
        if clang_include_dir:
            parameters.extend(['-isystem', clang_include_dir])
    elif platform.system() == 'Windows':
        parameters.extend(['-fms-compatibility', '-std=c++14'])
        
    if not any(it.startswith("-std=") for it in parameters):
        parameters.append('-std=c++11')
    for item in args:
        if item.startswith('-'):
            parameters.append(item)
        else:
            filenames.append(item)
    return parameters, filenames

class NoFilenamesError(ValueError):
    pass

def cleanup_output(output):
    cleaned = []
    prev = ''
    for line in output:
        if not line == prev:
            cleaned.append(line)
            prev = line
    return cleaned
    
def get_node_info(node):
    root = node
    prefix = ''
    while root.semantic_parent:
        root = root.semantic_parent
        if not root.kind == CursorKind.TRANSLATION_UNIT:
            prefix = root.spelling + '::' + prefix
    info = """{0:20} : {1:20}{2}{3}""".format(
       path.split(root.spelling)[1],
       node.result_type.spelling,
       prefix,
       node.displayname 
    )
    return info

def generate_all(args):
    global _AUTOGEN_FAIL_LIST
    _AUTOGEN_FAIL_LIST = []
    parameters, filenames = parse_args(args)
    if filenames:
        for filename in filenames:
            output = []
            parser = Parser(filename, parameters, output)
            parser.run()
            output = cleanup_output(output)
            outfile = path.split(filename)[1]
            with open("generated/" + outfile, "w") as wr:
                wr.writelines([(line+'\n') for line in output])
    else:
        raise NoFilenamesError

if __name__ == '__main__':
    try:
        args = []
        for arg in sys.argv[1:]:
            if arg.startswith('-'):
                args.append(arg)
            args.extend(glob(arg))
        generate_all(args)
    except NoFilenamesError:
        print("Usage:\n{} <clang options> [header files ...]".format(sys.argv[0]))
     
