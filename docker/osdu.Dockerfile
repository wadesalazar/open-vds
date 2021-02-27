# Use build_osdu_image.py in the project root
# The script will set the required environment variables to build the OSDU image for a specific tag

# The Docker build uses a multi-stage approach to first build the specific OpenVDS, then build the
# deployable based on the first stage output

# Global ARGS for multistage builds
ARG repo=https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/open-vds.git
ARG name="OpenVDS Import/Export Tool"
ARG tag=master

# Build stage
FROM ubuntu:20.04 as openvds-build-stage
ARG repo
ARG name
ARG tag

ENV DEBIAN_FRONTEND="noninteractive" TZ="Europe/London"
RUN apt-get update
RUN apt-get install -y vim cmake build-essential python3 python3-pip libboost-dev git doxygen openjdk-8-jdk libxml2-dev zlib1g-dev libcurl4-openssl-dev libuv1-dev uuid-dev gdb bash libssl-dev libboost-all-dev

RUN pip3 install ninja

COPY python/requirements-dev-with-docs.txt /tmp/requirements-dev.txt
RUN pip3 install -r tmp/requirements-dev.txt

COPY docker/build_scripts/generic_linux_build.sh /root/build_and_install_openvds.sh
RUN cd root && bash /root/build_and_install_openvds.sh ${tag} ${repo} -DBUILD_PYTHON=OFF -DBUILD_JAVA=OFF -DBUILD_DOCS=OFF -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF


# Deploy stage
FROM ubuntu:20.04 as openvds-deploy-stage
ARG repo
ARG name
ARG tag

LABEL maintainer="info@bluware.com" \
      vendor="Bluware Inc." \
      name=${name} \
      version=${tag} \
      description="This Docker Image imports/exports seismic data to/from VDS format" \
      license="Apache License, Version 2.0" \
      url="https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/open-vds" \
      vcs-url="https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/open-vds.git" \
      vcs-ref=${tag}

ENV DEBIAN_FRONTEND="noninteractive" TZ="Europe/London" \
    AWS_SHARED_CREDENTIALS_FILE="/data/aws.credentials" \
    GOOGLE_APPLICATION_CREDENTIALS="/data/gcp.json"

RUN apt update && \
    apt upgrade -y --no-install-recommends && \
    apt install -y --no-install-recommends ca-certificates libstdc++6 libgomp1 libcurl4 libcpprest libuv1 libboost-log1.71.0 libxml2 && \
    rm -rf /var/lib/apt/lists/* && \
    apt clean

COPY --from=openvds-build-stage /root/open-vds-install/ /usr/

COPY docker/osdu.entrypoint.sh /root/entrypoint.sh
ENTRYPOINT ["/root/entrypoint.sh"]

# Potential mount points, if needed for the processing of file-based data
# Current Working Directory (CWD) for the tools is /data
# Having multiple mountpoints allows for config/input/output on different mounted media
VOLUME ["/data", "/data_in", "/data_out"]
