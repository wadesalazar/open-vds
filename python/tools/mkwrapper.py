import os
import sys
import platform
import re
import textwrap

from clang import cindex
from clang.cindex import CursorKind
from collections import OrderedDict
from glob import glob
from threading import Thread, Semaphore
from multiprocessing import cpu_count

RECURSE_LIST = [
    CursorKind.TRANSLATION_UNIT,
    CursorKind.NAMESPACE,
    CursorKind.CLASS_DECL,
    CursorKind.STRUCT_DECL,
    CursorKind.ENUM_DECL,
    CursorKind.CLASS_TEMPLATE
]

PRINT_LIST = [
    CursorKind.CLASS_DECL,
    CursorKind.STRUCT_DECL,
    CursorKind.ENUM_DECL,
    CursorKind.ENUM_CONSTANT_DECL,
    CursorKind.CLASS_TEMPLATE,
    CursorKind.FUNCTION_DECL,
    CursorKind.FUNCTION_TEMPLATE,
    CursorKind.CONVERSION_FUNCTION,
    CursorKind.CXX_METHOD,
    CursorKind.CXX_BASE_SPECIFIER,
    CursorKind.CONSTRUCTOR,
    CursorKind.FIELD_DECL
]

PREFIX_BLACKLIST = [
    CursorKind.TRANSLATION_UNIT
]

PUBLIC_LIST = [
  cindex.AccessSpecifier.PUBLIC
]

PROTECTED_LIST = [
  cindex.AccessSpecifier.PUBLIC,
  cindex.AccessSpecifier.PROTECTED
]

PRIVATE_LIST = [
  cindex.AccessSpecifier.PUBLIC,
  cindex.AccessSpecifier.PROTECTED,
  cindex.AccessSpecifier.PRIVATE
]

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
    
def sanitize_name(name):
    name = re.sub(r'type-parameter-0-([0-9]+)', r'T\1', name)
    for k, v in CPP_OPERATORS.items():
        name = name.replace('operator%s' % k, 'operator_%s' % v)
    name = re.sub('<.*>', '', name)
    name = ''.join([ch if ch.isalnum() else '_' for ch in name])
    name = re.sub('_$', '', re.sub('_+', '_', name))
    return '__doc_' + name

def extract_nodes(filename, node, output, prefix=''):
    if not (node.location.file is None or
            os.path.samefile(d(node.location.file.name), filename)):
        return 0
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

def dump_node(n):
    print("name: {0:40} {2:20} displayname: {1}".format(n.spelling, n.displayname, str(n.kind).replace('CursorKind.', '')))

def getpyname(name):
    return name[0].lower() + name[1:]

def getparent(node, all):
    for n in all:
        if node in n.get_children():
            return n
    return None
    
def getchildren(node, all, kind=None, access_filter = PUBLIC_LIST):
    children = list(node.get_children())
    return [x for x in all if x in children and (kind is None or x.kind == kind) and x.access_specifier in access_filter]
    
def getbases(node, all):
    return getchildren(node, all, CursorKind.CXX_BASE_SPECIFIER)
    
def getoverloads(node, all, access_filter = PUBLIC_LIST):
    children = getchildren(node, all, node.kind, access_filter)
    return [x for x in all if x.kind == node.kind and x.spelling == node.spelling]

def getscope(node, all, output):
    p = getparent(node, all)
    while p:
        output.append(p)
        p = getparent(p, all)
    return output

def getfullname(node, all):
    scope = getscope(node, all, [])
    scope.reverse()
    prefix = ''
    for s in scope:
        prefix += s.spelling + '::'
    return "{}{}".format(prefix, node.spelling)

def getvarname(node, all):
    return getfullname(node, all).replace('::', '_') + '_'
    
def format_docstring_decl(fullname):
    return "OPENVDS_DOCSTRING({})".format(fullname.replace('::', '_'))
    
def resolve_overload_name(node, all):
    overloads = getoverloads(node, all)
    suffix = ''
    index = overloads.index(node)
    if index > 0:
        suffix += '_{}'.format(index + 1)
    return getfullname(node, all).replace('::', '_') + suffix
    
def generate_none(node, all, output, indent, parent_prefix, context):
    pass

def generate_field(node, all, output, indent, parent_prefix, context):
    overload_name = resolve_overload_name(node, all)
    code = """.def_readwrite("{}", native::{}, {});""".format(
        getpyname(node.spelling),
        getfullname(node, all),
        format_docstring_decl(overload_name)
    )
    output.append(indent + parent_prefix + code)

def generate_constructor(node, all, output, indent, parent_prefix, context):
    overload_name = resolve_overload_name(node, all)
    arglist = node.displayname[node.displayname.find('(') + 1:-1]
    code = """.def(py::init<{}>(), {});""".format(
       arglist,
       format_docstring_decl(overload_name)
    )
    output.append(indent + parent_prefix + code)

def generate_function(node, all, output, indent, parent_prefix, context):
    overload_name = resolve_overload_name(node, all)
    restype   = node.result_type.spelling
    arglist = node.displayname[node.displayname.find('('):]
    code = """.def("{}", static_cast<{}(*){}>(&native::{}), {});""".format(
       getpyname(node.spelling),
       restype,
       arglist,
       node.spelling,
       format_docstring_decl(overload_name)
    )
    output.append(indent + parent_prefix + code)
    
def generate_class(node, all, output, indent, parent_prefix, context):
    if node.get_definition():
        varname = getvarname(node, all)
        bases = getbases(node, all)
        basesdecl = ''
        for b in bases:
            basesdecl += ', native::{}'.format(b.get_definition().displayname)
        code = [ 
            '',
            indent + """py::class_<native::{}{}> \n{}  {}({},"{}", {});""".format(
                getfullname(node, all),
                basesdecl,
                indent,
                varname,
                parent_prefix,
                getfullname(node, all),
                format_docstring_decl(getfullname(node, all))
            ),
            ''
        ]
        for kind in CLASS_NODES:
            children = getchildren(node, all, kind)
            for child in children:
                generate = NODE_HANDLERS[kind]
                generate(child, all, code, indent, varname, context)
        output.extend(code)
        output.append('')
    else:
        pass

def generate_enumvalue(node, all, output, indent, parent_prefix, context):
    code = [
        indent + """{}.value("{}", native::{}, {});""".format(
            parent_prefix,
            node.spelling,
            getfullname(node, all),
            format_docstring_decl(getfullname(node, all))
        ),
    ]
    output.extend(code)
    
def generate_enum(node, all, output, indent, parent_prefix, context):
    if node.get_definition():
        varname = getvarname(node, all)
        bases = getbases(node, all)
        basesdecl = ''
        for b in bases:
            basesdecl += ', native::{}'.format(b.get_definition().displayname)
        code = [ 
            '',
            indent + """py::enum_<native::{}> \n{}  {}({},"{}", {});""".format(
                getfullname(node, all),
                indent,
                varname,
                parent_prefix,
                node.spelling,
                format_docstring_decl(getfullname(node, all))
            ),
            ''
        ]
        for kind in ENUM_NODES:
            children = getchildren(node, all, kind)
            for child in children:
                generate = NODE_HANDLERS[kind]
                generate(child, all, code, indent, varname, context)
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
          "filename":   self.filename,
          "parameters": self.parameters
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
    
def generate_all(args):
    parameters, filenames = parse_args(args)
    output = []
    if filenames:
        for filename in filenames:
            parser = Parser(filename, parameters, output)
            parser.run()
            output = cleanup_output(output)
            for l in output:
                print(l)
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
     