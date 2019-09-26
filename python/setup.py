#!/usr/bin/env python3

import os
import skbuild
import setuptools

class get_pybind_include(object):
    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        # postpone importing pybind11 until building actually happens
        import pybind11
        return pybind11.get_include(self.user)

pybind_includes = [
    str(get_pybind_include()),
    str(get_pybind_include(user = True))
]

skbuild.setup(
    name = 'openvds',
    packages = [
        'openvds',
    ],
    install_requires = [
        'numpy',
    ],
    setup_requires = [
        'pybind11 >= 2.2',
        'pytest-runner',
    ],
    tests_require = [
        'pytest',
    ],
    cmake_args = [
        '-DPYBIND11_INCLUDE_DIRS=' + ';'.join(pybind_includes),
    ],
    cmake_source_dir = 'openvds',

    # skbuild's test imples develop, which is pretty obnoxious instead, use a
    # manually integrated pytest.
    cmdclass = { 'test': setuptools.command.test.test },
)
