/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:02
 * @LastEditTime : 2020-01-08 20:36:29
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _NETWORK_H_
#define _NETWORK_H_

typedef struct network_params {
    char                *addr;
    int                 port;
    char                *ca;
} network_params_t;

typedef struct network {
    int                 socket;
    network_params_t    *network_params;
    int                 (*read)(struct network *, unsigned char *, int, int);
    int                 (*write)(struct network *, unsigned char *, int, int);
    int                 (*connect)(struct network *);
    void                (*disconnect)(struct network *);
} network_t;


int network_init(network_t* n);
int network_read(network_t* n, unsigned char* buf, int len, int timeout);
int network_write(network_t* n, unsigned char* buf, int len, int timeout);
int network_connect(network_t* n);
void network_release(network_t* n);

#endif
