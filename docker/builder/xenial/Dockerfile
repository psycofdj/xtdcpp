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
    python-pip \
    rsync \
    cmake \
    git \
    libboost-thread-dev \
    libboost-filesystem-dev \
    libboost-system-dev \
    libboost-regex-dev \
    plantuml \
    doxygen

RUN pip install pip --upgrade
RUN pip install coverxygen requests lxml
