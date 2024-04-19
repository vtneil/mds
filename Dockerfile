FROM debian:sid-slim as builder
LABEL authors="vivatsathorn"

WORKDIR /

# Essentials
RUN apt-get update && apt-get install -y \
    clang-18 \
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

ENV CC="/usr/bin/clang-18"
ENV CXX="/usr/bin/clang++-18"

FROM builder as or_tools_builder

# OR-Tools
RUN git clone --depth 1 https://github.com/google/or-tools.git

WORKDIR /or-tools/build/

RUN cmake .. -DBUILD_DEPS=ON && \
    make -j $(nproc) && \
    make install

WORKDIR /

RUN rm -rf ./or-tools

FROM or_tools_builder as compiler

WORKDIR /src/

COPY . .

RUN mkdir build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    make program && \
    cp ./program / && \
    rm -rf /src

FROM debian:sid-slim as runner

WORKDIR /

RUN apt-get update && apt-get install -y libstdc++6

COPY --from=compiler /usr/local/lib/ /usr/local/lib/

COPY --from=compiler /program /

RUN ldconfig /usr/local/lib/

ENTRYPOINT ["./program"]