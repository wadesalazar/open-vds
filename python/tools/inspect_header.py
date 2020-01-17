import os
import sys
import platform
from clang import cindex
from clang.cindex import CursorKind
from collections import OrderedDict


filename = 'tmpinclude/OpenVDS/VolumeDataAccess.h'
parameters = ['-x', 'c++', '-std=c++14', '-I../../src']

RECURSE_LIST = [
    CursorKind.TRANSLATION_UNIT,
    CursorKind.NAMESPACE,
    CursorKind.CLASS_DECL,
    CursorKind.STRUCT_DECL,
    CursorKind.ENUM_DECL,
    CursorKind.CLASS_TEMPLATE,
    CursorKind.CXX_METHOD,
    CursorKind.FUNCTION_DECL,
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
    CursorKind.FIELD_DECL,
    CursorKind.PARM_DECL,
]

PREFIX_BLACKLIST = [
    CursorKind.TRANSLATION_UNIT
]

def d(s):
    return s if isinstance(s, str) else s.decode('utf8')

def extract_nodes(filename, node, output=[], prefix=''):
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
    else:
#        print("Ignored: {}".format(node.kind))
        pass
    return output

def dump_node(n, prefix=''):
    print(prefix + "name: {0:40} {2:20} displayname: {1}".format(n.spelling, n.displayname, str(n.kind).replace('CursorKind.', '')))

def dump(all):
    i = 0;
    for node in all:
        dump_node(node, "{:3} ".format(i))
        i += 1
        
index = cindex.Index(cindex.conf.lib.clang_createIndex(False, True))
tu = index.parse(filename, parameters)
nodes = extract_nodes(filename, tu.cursor)
#s = [x for x in nodes if x.kind==CursorKind.STRUCT_DECL]
#s0=s[0]
#s1=s[1]
dump(nodes)


