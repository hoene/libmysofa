FROM ubuntu:14.04
MAINTAINER "christian.hoene@symonics.com"

RUN apt-get update && apt-get install -y --force-yes libnetcdfc7
ADD sofa2json /tmp/
ADD libnetcdf_c++4.so.* /usr/lib/
WORKDIR /tmp/

