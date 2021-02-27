#!/bin/sh
REPO_URL='https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/open-vds.git'
BRANCH_TAG='master'
if [ $# -gt 0 ]
  then
    BRANCH_TAG=$1
    shift
fi
if [ $# -gt 0 ]
  then
    REPO_URL=$1
    shift
fi
git clone --depth 1 --branch $BRANCH_TAG --single-branch $REPO_URL
cd open-vds
mkdir build_release
cd build_release
cmake -DCMAKE_BUILD_TYPE=Release "$@" -GNinja -DCMAKE_INSTALL_PREFIX=../../open-vds-install ..
ninja install
