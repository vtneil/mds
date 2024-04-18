FROM ubuntu:22.04 AS builder
LABEL authors="vivatsathorn"

#ARG DEBIAN_FRONTEND=noninteractive

WORKDIR /

# Essentials
RUN apt-get update && apt-get install -y \
    cmake \
    git \
    pkg-config \
    libssl-dev \
    wget \
    build-essential \
    autoconf \
    libtool \
    zlib1g-dev \
    lsb-release \
    curl \
    software-properties-common \
    gnupg \
    && rm -rf /var/lib/apt/lists/*

# LLVM Clang
RUN wget https://apt.llvm.org/llvm.sh && chmod +x llvm.sh && bash ./llvm.sh 18

# Set default compiler
ENV CC="/usr/bin/clang-18"
ENV CXX="/usr/bin/clang++-18"

# OR-Tools
RUN git clone --depth 1 https://github.com/google/or-tools.git

RUN cd or-tools && mkdir build && cd build && \
    cmake .. -DBUILD_DEPS=ON && \
    make -j $(nproc) && \
    make install

FROM builder as compiler

WORKDIR /app/

COPY . .

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make program

FROM compiler as runner

WORKDIR /app/

ENTRYPOINT ["./build/program"]