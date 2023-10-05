FROM ubuntu:22.04

RUN apt-get update \
        && apt-get install -y \
        git \
        wget \
        mercurial \
        gcc \
        g++ \
        vim \
        cmake \
        openjdk-17-jdk \
        tar \
        bzip2 \
        unzip \
        && mkdir -p /workspace

WORKDIR /workspace
RUN wget https://www.nsnam.org/releases/ns-allinone-3.37.tar.bz2 \
        && tar -jxvf ns-allinone-3.37.tar.bz2 \
        && wget https://github.com/nsol-nmsu/brite-patch/archive/refs/heads/master.zip \
        && unzip master.zip \
        && hg clone http://code.nsnam.org/BRITE \
        && rm master.zip \
        && rm ns-allinone-3.37.tar.bz2

WORKDIR /workspace/BRITE
RUN make

WORKDIR /workspace/ns-allinone-3.37/ns-3.37
COPY ./contrib /workspace/ns-allinone-3.37/ns-3.37/contrib
RUN ./ns3 configure --with-brite=/workspace/BRITE --enable-examples --enable-tests \
        && ./ns3 build

WORKDIR /workspace/brite-patch-master
RUN make

WORKDIR /workspace