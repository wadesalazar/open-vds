FROM centos:7

RUN yum install -y centos-release-scl epel-release

RUN yum install -y rh-python36-scldevel rh-python36-numpy rh-python36-python-pip devtoolset-8

RUN curl -OL https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-Linux-x86_64.tar.gz
RUN tar xzvf cmake-3.17.2-Linux-x86_64.tar.gz -C /opt
RUN ln -s /opt/cmake-3.17.2-Linux-x86_64/bin/* /usr/bin/
RUN scl enable rh-python36 devtoolset-8 'pip install ninja scikit-build'
RUN yum install -y git doxygen java-1.8.0-openjdk-devel libxml2-devel zlib-devel boost169-devel openssl-devel libcurl-devel libuv-devel libuuid-devel
