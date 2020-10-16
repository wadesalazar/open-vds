#!/bin/sh
git clone --depth 1 --single-branch https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/open-vds.git
cd open-vds
mkdir build_release
cd build_release
cmake -DCMAKE_BUILD_TYPE=Release -GNinja -DCMAKE_INSTALL_PREFIX=../../open-vds-install ..
ninja install
