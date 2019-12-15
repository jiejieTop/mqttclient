/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:02
 * @LastEditTime: 2019-12-15 15:53:55
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _NETWORK_H_
#define _NETWORK_H_

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

#include "timer.h"

typedef struct connect_params {
    char                *addr;
    int                 port;
    char                *ca;
} connect_params_t;


typedef struct network {
    int                 socket;
    connect_params_t    connect_params;
    int                 (*read)(struct network *, unsigned char *, int, int);
    int                 (*write)(struct network *, unsigned char *, int, int);
    int                 (*connect)(struct network *);
    void                (*disconnect)(struct network *);
} network_t;


int network_init(network_t *n, connect_params_t *connect_params);
int network_read(network_t *n, unsigned char *buf, int len, int timeout);
int network_write(network_t *n, unsigned char *buf, int len, int timeout);
int network_connect(network_t *n);
void network_disconnect(network_t *n);

#endif
