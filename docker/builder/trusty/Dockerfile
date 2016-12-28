FROM ubuntu:14.04

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
    doxygen

RUN pip install pip --upgrade
RUN pip install coverxygen requests lxml
RUN mkdir -p /usr/share/plantuml/ && wget -O /usr/share/plantuml/plantuml.jar 'http://downloads.sourceforge.net/project/plantuml/plantuml.jar'
