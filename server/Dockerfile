FROM gcc:latest

RUN apt-get update && \
    apt-get install -y \
    build-essential \
    cmake

WORKDIR /tmp/boost
RUN wget --max-redirect 4 -q https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.gz && \
    tar xf boost_1_79_0.tar.gz && \
    cd boost_1_79_0 && \
    ./bootstrap.sh && \
    ./b2 install

RUN apt-get install libpqxx-dev -y

WORKDIR /app

COPY . .
COPY Makefile .
RUN make

CMD ["/app/bin/server"]
