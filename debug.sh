#!/bin/bash
docker stop gdb
docker rm gdb
docker run -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name gdb agodio/itba-so:1.0
docker stop gdb
docker rm gdb