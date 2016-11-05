FROM ubuntu:14.04

RUN apt-get update && \
    apt-get install -y g++ libboost-dev libcppunit-dev graphviz xsltproc lcov cloc cppcheck valgrind

RUN apt-get install -y python3-pip
RUN pip3 install pip --upgrade
RUN pip3 install coverxygen

RUN apt-get install -y rsync cmake git libboost-thread-dev libboost-filesystem-dev libboost-system-dev libboost-regex-dev doxygen wget default-jre
RUN mkdir -p /usr/share/plantuml/ && wget -O /usr/share/plantuml/plantuml.jar 'http://downloads.sourceforge.net/project/plantuml/plantuml.jar' 
