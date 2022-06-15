#!/bin/bash

current_pwd=$(
    cd "$(dirname "$0")"
    pwd
)

mkdir -p $current_pwd/libmqttclient $current_pwd/libmqttclient/include $current_pwd/libmqttclient/lib

cd $current_pwd/libmqttclient

if [ ! -f "Makefile" ]; then

    cat <<-EOF >Makefile
CC=\$(CROSS_COMPILE)gcc

SRC = \$(wildcard $current_pwd/*.c \\
        $current_pwd/common/*.c \\
        $current_pwd/common/log/*.c \\
        $current_pwd/common/log/arch/linux/*.c \\
        $current_pwd/network/mbedtls/library/*.c \\
        $current_pwd/network/mbedtls/wrapper/*.c \\
        $current_pwd/mqtt/*.c \\
        $current_pwd/mqttclient/*.c \\
        $current_pwd/network/*.c \\
        $current_pwd/platform/linux/*.c \\
        )

INC =   -lpthread \\
        -I$current_pwd/common \\
        -I$current_pwd/common/log \\
        -I$current_pwd/network/mbedtls/include \\
        -I$current_pwd/network/mbedtls/include/mbedtls \\
        -I$current_pwd/network/mbedtls/wrapper \\
        -I$current_pwd/mqtt \\
        -I$current_pwd/mqttclient \\
        -I$current_pwd/network \\
        -I$current_pwd/platform/linux \\
        -I$current_pwd/test

OBJS = \$(patsubst %.c, %.o, \$(SRC))

FLAG = -g -fpic -I. -Iinclude \$(INC) 
TARGET = libmqttclient.so

EOF

    echo -e "\n\$(TARGET):\$(OBJS)" >>Makefile
    echo -e "\t\$(CC) -shared \$^ -o \$@" >>Makefile

    echo -e "\n%.o:%.c" >>Makefile
    echo -e "\t\$(CC) -c \$(FLAG) \$^ -o \$@" >>Makefile

    echo -e "\nclean:" >>Makefile
    echo -e "\trm -rf \$(TARGET) \$(OBJS)" >>Makefile

    echo -e "\ninstall:" >>Makefile
    echo -e "\tsudo cp -rdf \$(TARGET) /usr/lib/." >>Makefile

    echo -e "\nremove:" >>Makefile
    echo -e "\tsudo rm -rdf /usr/lib/\$(TARGET)" >>Makefile

    echo -e "\n.PHONY:clean" >>Makefile

fi

mkdir -p $current_pwd/libmqttclient/include/test/.
mkdir -p $current_pwd/libmqttclient/include/mqtt/.
mkdir -p $current_pwd/libmqttclient/include/common/.
mkdir -p $current_pwd/libmqttclient/include/network/.
mkdir -p $current_pwd/libmqttclient/include/mqttclient/.
mkdir -p $current_pwd/libmqttclient/include/common/log/.
mkdir -p $current_pwd/libmqttclient/include/platform/linux/.
mkdir -p $current_pwd/libmqttclient/include/mbedtls/.
mkdir -p $current_pwd/libmqttclient/include/mbedtls/wrapper/.

cp -r $current_pwd/test/*.h $current_pwd/libmqttclient/include/test/.
cp -r $current_pwd/mqtt/*.h $current_pwd/libmqttclient/include/mqtt/.
cp -r $current_pwd/common/*.h $current_pwd/libmqttclient/include/common/.
cp -r $current_pwd/network/*.h $current_pwd/libmqttclient/include/network/.
cp -r $current_pwd/mqttclient/*.h $current_pwd/libmqttclient/include/mqttclient/.
cp -r $current_pwd/common/log/*.h $current_pwd/libmqttclient/include/common/log/.
cp -r $current_pwd/platform/linux/*.h $current_pwd/libmqttclient/include/platform/linux/.
cp -r $current_pwd/network/mbedtls/include/mbedtls/* $current_pwd/libmqttclient/include/mbedtls/.
cp -r $current_pwd/network/mbedtls/wrapper/*.h $current_pwd/libmqttclient/include/mbedtls/wrapper/.

if [ " $1" == " " ]; then
    cd $current_pwd/libmqttclient
    make
    mv libmqttclient.so $current_pwd/libmqttclient/lib/.
    make clean

elif [ "$1" == "remove" ]; then
    make remove
fi
