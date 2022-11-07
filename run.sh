#!/bin/bash
# --rtc base=localtime para q este correctamente seteado el RTC

# abro docker y ejecuto los make necesarios
docker run  -e "GCC_COLORS=\"error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01\"" -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name dockerArqui agodio/itba-so:1.0
docker exec -it dockerArqui make clean    -C /root/Toolchain
docker exec -it dockerArqui make all      -C /root/Toolchain
docker exec -it dockerArqui make clean    -C /root/
if [ "$1" == "gdb" ]
then
    docker exec -it dockerArqui make gdb      -C /root/
else
    docker exec -it dockerArqui make all      -C /root/
fi
docker stop dockerArqui
docker rm dockerArqui

if [ "$1" == "gdb" ]
then
    # modo debug
    qemu-system-x86_64 -S -s --rtc base=localtime -hda Image/x64BareBonesImage.qcow2 -m 512 -d int -monitor stdio
    # docker run -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name gdb agodio/itba-so:1.0
    # adentro del docker correr cd root
    # despues gdb
    # docker stop gdb
    # docker rm gdb
else
    # abro la imagen en qemu
    qemu-system-x86_64 --rtc base=localtime -hda Image/x64BareBonesImage.qcow2 -m 512
fi
