FROM ubuntu:latest
RUN apt-get update --fix-missing && apt-get install -y \
    g++-7 \
    make \
    git \
    openssl
RUN cd / && git clone https://github.com/meilier/pca.git \
    && rm /binsh \
    && ln -s /bin/bash /bin/sh