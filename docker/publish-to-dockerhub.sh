#!/bin/bash -e

docker login
docker build -t webis/netspeak:4.1.1 .
docker push webis/netspeak:4.1.1

