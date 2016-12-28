FROM ubuntu:16.04

RUN apt-get update -y && \
    apt-get install -y \
    g++ \
    libboost-dev \
    libcppunit-dev \
    graphviz \
    xsltproc \
    lcov \
    cloc \
    cppcheck \
    valgrind \
    python3-pip \
    rsync \
    cmake \
    git \
    libboost-thread-dev \
    libboost-filesystem-dev \
    libboost-system-dev \
    libboost-regex-dev \
    plantuml \
    doxygen

RUN pip3 install pip --upgrade
RUN pip3 install  coverxygen requests
