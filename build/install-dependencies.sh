#!/bin/bash

set -e

if (( $EUID != 0 )); then
    echo "Please run as root"
    exit 1
fi

# go to the dir of this script
cd "$(dirname "$0")"

apt-get update --allow-releaseinfo-change
apt-get install make clang build-essential libboost-filesystem-dev libboost-system-dev libboost-test-dev libboost-program-options-dev libboost-stacktrace-dev libicu-dev libcmph-dev libaio-dev libboost-regex-dev libboost-date-time-dev python3-dev -y


bash ./env/install-grpc.sh


mkdir -p ./dependencies
chmod a+rw -R ./dependencies
cd ./dependencies


# Download pybind11
if [ ! -d ./pybind11-2.9.0 ]; then
    echo "Downloading pybind11"

    apt-get install unzip wget -y
    wget -O pybind.zip 'https://github.com/pybind/pybind11/archive/refs/tags/v2.9.0.zip'
    unzip pybind.zip
    rm pybind.zip
    chmod a+rw ./**/*
else
    echo "pybind11 already present"
fi


if [[ "$1" != "ci" ]]; then

    # Download protoc-gen-grpc-web, a protoc plugin to generate gRPC web code
    if [ ! -f ./protoc-gen-grpc-web ]; then

        wget -O protoc-gen-grpc-web 'https://github.com/grpc/grpc-web/releases/download/1.2.0/protoc-gen-grpc-web-1.2.0-linux-x86_64'
        chmod a+rwx ./protoc-gen-grpc-web

    fi

    # Download protoc-gen-grpc-java, a protoc plugin to generate gRPC Java code
    if [ ! -f ./protoc-gen-grpc-java ]; then

        wget -O protoc-gen-grpc-java 'https://repo1.maven.org/maven2/io/grpc/protoc-gen-grpc-java/1.32.2/protoc-gen-grpc-java-1.32.2-linux-x86_64.exe'
        chmod a+rwx ./protoc-gen-grpc-java

    fi

fi
