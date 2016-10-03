FROM ubuntu:16.04
MAINTAINER Xavier MARCELET <xavier@marcelet.com>

RUN apt-get update && apt-get install -y --force-yes \
    cmake \
    doxygen \
    lcov \
    xsltproc \
    graphviz \
    cppcheck \
    cloc \
    libboost-all-dev \
    python3-pip \
    libcppunit-dev \
    texlive-font-utils \
    plantuml \
    valgrind \
    git \
    zlib1g-dev
RUN pip3 install coverxygen
RUN pip3 install lxml
RUN pip3 install requests
