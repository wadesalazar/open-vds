FROM docker.io/centos:8

#RUN yum install -y centos-release-scl epel-release
#
#RUN yum install -y rh-python36-scldevel rh-python36-numpy rh-python36-python-pip devtoolset-8
#
#SHELL [ "/usr/bin/scl", "enable", "devtoolset-8", "rh-python36" ]
#
RUN dnf -y install dnf-plugins-core
RUN dnf -y install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm
RUN dnf config-manager --set-enabled PowerTools epel-modular
RUN yum install -y python3 gcc boost-devel git doxygen java-1.8.0-openjdk-devel libxml2-devel zlib-devel boost169-devel openssl-devel libcurl-devel libuv libuuid-devel
RUN dnf -y module enable libuv
RUN yum install -y libuv-devel
RUN yum -y group install "Development Tools"

RUN curl -OL https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-Linux-x86_64.tar.gz
RUN tar xzvf cmake-3.17.2-Linux-x86_64.tar.gz -C /opt
RUN ln -s /opt/cmake-3.17.2-Linux-x86_64/bin/* /usr/bin/

RUN python3 -m pip install scikit-build ninja
#
## cmake -DCMAKE_BUILD_TYPE=Debug -GNinja ..
