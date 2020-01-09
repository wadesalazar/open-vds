#!/usr/bin/env python3

import sys

try:
    from skbuild import setup
except ImportError:
    print('scikit-build is required to build from source.', file=sys.stderr)
    print('Please run:', file=sys.stderr)
    print('', file=sys.stderr)
    print('  python -m pip install scikit-build')
    sys.exit(1)

setup(
    name="openvds",
    version="0.1",
    description="Open-source implementation of the Volume Data Store (VDS) standard for fast random access to multi-dimensional volumetric data",
    author='The Open Group / Bluware, Inc.',
    license="Apache License, Version 2.0",
    packages=['openvds'],
    package_dir={'': 'python'}
)
