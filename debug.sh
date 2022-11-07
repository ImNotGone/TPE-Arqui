#!/bin/bash
docker run -v ${PWD}:/root --security-opt seccomp:unconfined -ti --rm --name gdb agodio/itba-so:1.0

