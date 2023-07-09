FROM gcc:latest as server

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

WORKDIR /app/server

COPY server .
RUN make
RUN mkdir -p /app/bin && \
    cp bin/server /app/bin/server

COPY .env /app/.env

FROM node:latest as interface

WORKDIR /app/interface

COPY interface .
RUN yarn install && \
    yarn build-ts && \
    yarn build-sass

RUN cp -r /app/interface/dist/style/* /app/interface/dist && \
    rm -rf /app/interface/dist/style

FROM gcc:latest

COPY --from=server /usr/local/include/boost /usr/local/include/boost
COPY --from=server /usr/local/lib /usr/local/lib

COPY --from=server /app/bin/server /app/bin/server
COPY .env /app/.env
COPY --from=interface /app/interface/dist /app/interface/dist
COPY --from=interface /app/interface/assets /app/interface/assets

COPY interface/index.html /app/interface/index.html

WORKDIR /app

RUN apt-get update && \
    apt-get install libpqxx-dev -y && \
    wget https://raw.githubusercontent.com/vishnubob/wait-for-it/master/wait-for-it.sh && \
    chmod +x wait-for-it.sh

CMD ["./wait-for-it.sh", "database:5432", "--", "/app/bin/server"]
