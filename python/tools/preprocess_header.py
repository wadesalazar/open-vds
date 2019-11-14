"""Preprocess OpenVDS headers for mkdoc.py"""

import os
import sys
import re
from glob import glob

include_prefix = "tmpinclude/OpenVDS/"

def process_comments(lines):
    result = []
    is_inside_doc_comment = False
    comment = ""
    for line in lines:
        line = line.decode().lstrip()
        if line.startswith('/// <summary>'):
            is_inside_doc_comment = True
            comment = ""
        if line.startswith('/// '):
            if is_inside_doc_comment:
                s = line[4:]
                comment += s
            else:
                result.append(line)
        else:
            if is_inside_doc_comment:
                result.append('/*!\n')
                s = comment
                s = re.sub(r'<summary>(.*?)</summary>', r'\1', s, flags=re.DOTALL)
                s = re.sub(r'<param name="(.*?)">(.*?)</param>', r'\\param \1 \2', s, flags=re.DOTALL)
                s = re.sub(r'<returns>(.*?)</returns>', r'\\return\1', s, flags=re.DOTALL)
                comment = s.replace('\n', '\n  ')
                result.extend(comment.splitlines(keepends=True))
                result.append('*/\n')
            result.append(line)
            is_inside_doc_comment = False
    result = [line.encode() for line in result]
    return result
    
def preprocess_headers():
    index = 0
    targetdir = include_prefix if include_prefix.endswith("/") else include_prefix + "/" 
    while index >= 0 and index < len(targetdir):
        index = targetdir.find("/", index)
        if index > 0:
            try:
                os.mkdir(targetdir[:index])
            except:
                pass
            index += 1
    files = []
    for arg in sys.argv[1:]:
        files.extend(glob(arg))
    if files is not None:
        for file in files:
            file = file.replace("\\", "/")
            print("Processing: {}".format(file))
            with open(file, "rb") as headerfile:
                lines = headerfile.readlines()
            result = process_comments(lines)
            outfile = file[file.rfind("/"):]
            with open(targetdir + outfile, "wb") as f:
                f.writelines(result)
            print("Wrote: {}".format(outfile))
    else:
        raise ValueError("No files to process")

if __name__ == "__main__":
    preprocess_headers()

