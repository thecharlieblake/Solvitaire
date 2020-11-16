FROM ubuntu:18.04

WORKDIR /home/Solvitaire/

COPY . /home/Solvitaire/

RUN apt-get update \
&& apt-get install -y \
gcc-6 \
cmake \
libboost-all-dev \
git \
python3 \
vim \
parallel

RUN cmake [-G "CodeBlocks - Unix Makefiles"] \
-DCMAKE_BUILD_TYPE=RELEASE \
-Bcmake-build-release -H. \
&& cmake [-G "CodeBlocks - Unix Makefiles"] \
-DCMAKE_BUILD_TYPE=DEBUG \
-Bcmake-build-debug -H.

RUN cmake --build cmake-build-release \
&& cmake --build cmake-build-debug

WORKDIR /home/
RUN mv /home/Solvitaire/ /home/.install/
WORKDIR /home/Solvitaire/

CMD /bin/bash
