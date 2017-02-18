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
    zlib1g-dev \
    wget

RUN apt-get update && apt-get install -y --force-yes python-pip netcat-openbsd
RUN pip3 install coverxygen lxml requests
RUN pip install coverxygen lxml requests
