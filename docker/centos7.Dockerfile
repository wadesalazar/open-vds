FROM docker.io/centos:7

RUN yum install -y centos-release-scl epel-release

RUN yum install -y rh-python36-scldevel rh-python36-numpy rh-python36-python-pip devtoolset-8

SHELL [ "/usr/bin/scl", "enable", "devtoolset-8", "rh-python36" ]

RUN curl -OL https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-Linux-x86_64.tar.gz
RUN tar xzvf cmake-3.17.2-Linux-x86_64.tar.gz -C /opt
RUN ln -s /opt/cmake-3.17.2-Linux-x86_64/bin/* /usr/bin/
RUN pip install ninja scikit-build
RUN yum install -y git doxygen java-1.8.0-openjdk-devel libxml2-devel zlib-devel boost169-devel openssl-devel libcurl-devel libuv-devel libuuid-devel

COPY docker/build_scripts/centos7_build.sh /build_and_install_openvds.sh
# cmake -DCMAKE_BUILD_TYPE=Debug -GNinja -DBOOST_INCLUDEDIR=/usr/include/boost169 -DBOOST_LIBRARYDIR=/usr/lib64/boost169 ..
