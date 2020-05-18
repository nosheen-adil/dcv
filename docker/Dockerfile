FROM dcv:latest
SHELL ["/bin/bash", "-c"]

RUN apt-get update && \
	 apt-get install -yq --no-install-recommends \
	 apt-utils \
	 ca-certificates \
	 sudo \
	 vim \
	 cmake \
	 git \
	 curl \
	 wget \
	 autoconf \
	 autogen \
	 automake \
	 libtool \
	 scons \
	 make \
	 gcc \
	 g++ \
	 unzip \
	 pkg-config \
	 && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp
RUN echo "Installing libzmq" && \
  git clone --depth 1 https://github.com/zeromq/libzmq.git
WORKDIR /tmp/libzmq
RUN mkdir build
WORKDIR /tmp/libzmq/build
RUN cmake -DZMQ_BUILD_TESTS=OFF .. && make -j$(nproc) install

WORKDIR /tmp
RUN echo "Installing cppzmq" && \
  git clone --depth 1 https://github.com/zeromq/cppzmq.git
WORKDIR /tmp/cppzmq
RUN mkdir build
WORKDIR /tmp/cppzmq/build
RUN cmake -DCPPZMQ_BUILD_TESTS=OFF .. && make -j$(nproc) install

WORKDIR /tmp
RUN echo "Installing google flatbuffers" && \
	git clone https://github.com/google/flatbuffers.git
WORKDIR /tmp/flatbuffers
RUN mkdir build
WORKDIR /tmp/flatbuffers/build
RUN cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DFLATBUFFERS_BUILD_TESTS=OFF .. && make -j$(nproc) install
RUN sudo mv /tmp/flatbuffers/build/flatc /usr/local/bin/flatc
RUN chmod +x /usr/local/bin/flatc

WORKDIR /tmp
RUN echo "Installing spdlog" && \
 git clone https://github.com/gabime/spdlog.git
WORKDIR /tmp/spdlog
RUN mkdir build 
WORKDIR /tmp/spdlog/build
RUN cmake .. && make -j$(nproc) install

WORKDIR /tmp
RUN echo "Installing rapidjson" && \
	git clone https://github.com/Tencent/rapidjson.git
RUN mkdir build 
WORKDIR /tmp/rapidjson/build
RUN cmake -DRAPIDJSON_BUILD_DOC=OFF -DRAPIDJSON_BUILD_EXAMPLES=OFF -DRAPIDJSON_BUILD_TESTS=OFF .. && make -j$(nproc) install

WORKDIR /tmp
RUN echo "Installing curl" && \
  sudo apt-get update && sudo apt-get install libssl-dev && git clone https://github.com/curl/curl.git
WORKDIR /tmp/curl
RUN mkdir build 
WORKDIR /tmp/curl/build
RUN cmake -DCURL_STATICLIB=ON .. && make -j$(nproc) install

WORKDIR /tmp
RUN echo "Installing yaml-cpp" && \
  git clone https://github.com/jbeder/yaml-cpp.git
WORKDIR /tmp/yaml-cpp
RUN mkdir build
WORKDIR /tmp/yaml-cpp/build
RUN cmake -DYAML_CPP_BUILD_TESTS=OFF .. && make -j$(nproc) install

WORKDIR /
RUN rm -rf /tmp/*