FROM ubuntu:24.04

RUN apt-get -y update \
    && apt-get -y install wget lsb-release software-properties-common gnupg \
    && wget https://apt.llvm.org/llvm.sh \
    && chmod +x llvm.sh \
    && ./llvm.sh 21 \
    && apt-get -y install cmake \
    && apt-get -y install llvm-21 llvm-21-dev llvm-21-tools clang-21 \
    && mkdir ${HOME}/cgra \
    && cd ${HOME}/cgra \
    && apt-get -y install git \
    && git clone https://github.com/tancheng/CGRA-Mapper.git \
    && apt-get -y install build-essential \
    && cd CGRA-Mapper && mkdir build && cd build && cmake .. \
    && make

CMD ["tail", "-f", "/dev/null"]
