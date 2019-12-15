/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 13:39:00
 * @LastEditTime: 2019-12-16 01:59:23
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _NETTYPE_H_
#define _NETTYPE_H_
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "network.h"
#include "error.h"

int platform_nettype_read(network_t *n, unsigned char *buf, int len, int timeout);
int platform_nettype_write(network_t *n, unsigned char *buf, int len, int timeout);
int platform_nettype_connect(network_t* n);
void platform_nettype_disconnect(network_t* n);

#endif