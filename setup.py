#!/usr/bin/env python3

import sys
import os
import re
from pathlib import Path

if sys.version_info[0] < 3:
    raise Exception("OpenVDS only supports Python 3")

openvds_version=""
dir_path = os.path.dirname(os.path.realpath(__file__))
with open(dir_path + "/CMakeLists.txt", "r") as file:
    for line in file:
         if re.search("project.*VERSION", line):
             openvds_version = re.sub("[^0-9|.]*", "", line)
             break

if not openvds_version:
    eprint("Fatal error: Failed to parse version from CMakeLists.txt")
    exit(1)

try:
    from skbuild import setup
except ImportError:
    print('scikit-build is required to build from source.', file=sys.stderr)
    print('Please run:', file=sys.stderr)
    print('', file=sys.stderr)
    print('  python -m pip install scikit-build')
    sys.exit(1)

python_root_path = "-DPython3_ROOT_DIR={}".format(Path(sys.executable).parent.absolute().as_posix())
setup(
    name="openvds",
    version=openvds_version,
    description="Open-source implementation of the Volume Data Store (VDS) standard for fast random access to multi-dimensional volumetric data",
    author='The Open Group / Bluware, Inc.',
    license="Apache License, Version 2.0",
    packages=['openvds'],
    package_dir={'': 'python'},
    cmake_args=[python_root_path]
)
