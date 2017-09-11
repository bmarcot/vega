#
# Vega Continuous Integration Image
#
# Copyright (c) 2017 Baruch Marcot
#

FROM ubuntu:16.04
MAINTAINER Baruch Marcot <juz4m@hotmail.com>

# Install essential packages (required to add an external PPA)
RUN apt-get update -qq && apt-get install -y software-properties-common

# Add GNU ARM Embedded Toolchain repository to apt
RUN add-apt-repository ppa:team-gcc-arm-embedded/ppa

# Install other packages
RUN apt-get update -qq && apt-get install -y \
    gcc					     \
    gcc-arm-embedded			     \
    genromfs				     \
    git					     \
    make				     \
    python3				     \
    qemu-system-arm

WORKDIR /vega
