FROM ubuntu:20.04

ENV DEBIAN_FRONTEND="noninteractive" TZ="Europe/London"
RUN apt-get update
RUN apt-get install -y vim cmake build-essential python3 python3-pip libboost-dev git doxygen openjdk-8-jdk libxml2-dev zlib1g-dev libcurl4-openssl-dev libuv1-dev uuid-dev gdb bash libssl-dev libboost-all-dev

RUN pip3 install ninja

COPY python/requirements-dev-with-docs.txt /tmp/requirements-dev.txt
RUN pip3 install -r tmp/requirements-dev.txt

COPY docker/build_scripts/generic_linux_build.sh /build_and_install_openvds.sh
