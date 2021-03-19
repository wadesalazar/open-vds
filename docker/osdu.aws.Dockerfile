# This Docker build simply uses the latest image of the openvds-ingestion image, and replaces
# the sdapi library with the one built for AWS.
#
# This is only a temporary measure, until AWS's code changes to sdapi can be made public.

FROM community.opengroup.org:5555/osdu/platform/domain-data-mgmt-services/seismic/open-vds/openvds-ingestion:latest

WORKDIR /root
COPY binaries/aws_sdapi_linux64_3.1.32.tar.gz /root/aws_sdapi_linux64_3.1.32.tar.gz
RUN tar zxf /root/aws_sdapi_linux64_3.1.32.tar.gz && \
    cp /root/sdapi-3.1.32/lib/linux64/libsdapi.so.3.1.32 /usr/lib/libsdapi.so.0.0.0
