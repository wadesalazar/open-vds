#!/bin/sh
git clone --depth 1 --single-branch https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/open-vds.git
cd open-vds
mkdir build_release
cd build_release
scl enable rh-python36 devtoolset-8 "cmake -DCMAKE_BUILD_TYPE=Release -GNinja -DBOOST_INCLUDEDIR=/usr/include/boost169 -DBOOST_LIBRARYDIR=/usr/lib64/boost169 -DCMAKE_INSTALL_PREFIX=../../open-vds-install .."
scl enable rh-python36 devtoolset-8 "ninja install"
