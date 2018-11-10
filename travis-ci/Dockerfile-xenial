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
    wget \
    llvm-4.0-dev \
    libclang-4.0-dev

RUN apt-get update && apt-get install -y --force-yes python-pip netcat-openbsd
RUN pip3 install coverxygen lxml requests
RUN pip install coverxygen lxml requests
RUN wget https://github.com/pmd/pmd/releases/download/pmd_releases%2F5.7.0/pmd-bin-5.7.0.zip
RUN unzip -d /usr/share pmd-bin-5.7.0.zip
ADD https://github.com/include-what-you-use/include-what-you-use/archive/clang_4.0.tar.gz .
RUN apt-get update && apt-get install -y --force-yes libncurses-dev python3-mako
RUN tar xvzf clang_4.0.tar.gz && \
 cd include-what-you-use-clang_4.0 && \
 cmake -DIWYU_LLVM_ROOT_PATH=/usr/lib/llvm-4.0 . && \
 make -j2 && make install
