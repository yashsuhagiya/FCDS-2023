FROM tudinfse/cds_server:latest

WORKDIR /etc
MAINTAINER Yash Suhagiya <yasu279d@msx.tu-dresden.de>

RUN apt-get update && apt-get upgrade -y --fix-missing > /dev/null
RUN apt-get install build-essential -y > /dev/null
RUN apt-get install apt-utils gcc libncurses5-dev libssl-dev openssl libnet1-dev libpcap0.8-dev -y


COPY ./cds_server.json cds_server.json

#COPY ./hps/mopp-2018-t0-harmonic-progression-sum.cpp mopp-2018-t0-harmonic-progression-sum.cpp
#COPY ./hps/Makefile Makefile
#RUN make all

COPY ./password/mopp-2019-t2-brute-force-password-cracking.c mopp-2019-t2-brute-force-password-cracking.c
COPY ./password/Makefile Makefile
RUN make all

COPY ./levenshtein/mopp-2018-t2-levenshtein.cc mopp-2018-t2-levenshtein.cc
COPY ./levenshtein/ThreadPool.h ThreadPool.h
COPY ./levenshtein/Makefile Makefile
RUN make all
