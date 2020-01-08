/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 13:39:00
 * @LastEditTime : 2020-01-08 20:24:26
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _PLATFORM_NETTYPE_TCP_H_
#define _PLATFORM_NETTYPE_TCP_H_
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "network.h"
#include "error.h"

int platform_nettype_read(network_t *n, unsigned char *buf, int len, int timeout);
int platform_nettype_write(network_t *n, unsigned char *buf, int len, int timeout);
int platform_nettype_connect(network_t* n);
void platform_nettype_disconnect(network_t* n);

#endif