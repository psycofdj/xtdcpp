FROM ubuntu:16.04

RUN apt-get update && \
    apt-get install -y g++ libboost-dev libcppunit-dev graphviz xsltproc lcov cloc cppcheck valgrind

RUN apt-get install -y python3-pip
RUN pip3 install pip --upgrade
RUN pip3 install coverxygen
