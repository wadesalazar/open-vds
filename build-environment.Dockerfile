FROM alpine:latest

# Install the various required packages
RUN apk add python3 python3-dev gcc g++ cmake make ninja git ccache \
            libxml2-dev boost-dev zlib-dev openssl-dev curl-dev libuv-dev \
            util-linux-dev # This package contains the uuid development headers

# Copy the python requirement list into the container and download the packages
COPY python/requirements-dev.txt /tmp/requirements-dev.txt
RUN pip3 install -r tmp/requirements-dev.txt
