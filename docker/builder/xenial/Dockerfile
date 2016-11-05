FROM ubuntu:16.04

RUN apt-get update && \
    apt-get install -y g++ libboost-dev libcppunit-dev graphviz xsltproc lcov cloc cppcheck valgrind

RUN apt-get install -y python3-pip rsync
RUN pip3 install pip --upgrade
RUN pip3 install coverxygen
RUN apt-get install -y cmake git
RUN apt-get install -y libboost-thread-dev libboost-filesystem-dev libboost-system-dev libboost-regex-dev
RUN apt-get install -y plantuml
RUN apt-get install -y doxygen

