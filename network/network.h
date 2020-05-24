/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:02
 * @LastEditTime: 2020-05-20 23:16:11
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "mqtt_defconfig.h"

#define     NETWORK_CHANNEL_TCP     0
#define     NETWORK_CHANNEL_TLS     1

typedef struct network {
    int                         socket;
    int                         channel;        /* tcp or tls */
    const char                  *host;
    const char                  *port;
    const char		            *ca_crt;
    size_t 		                ca_crt_len;
    unsigned int                timeout_ms;            // SSL handshake timeout in millisecond
    void                        *nettype_tls_params;
} network_t;

int network_init(network_t *n, const char *host, const char *port, const char *ca);
int network_set_host_port(network_t* n, char *host, char *port);
int network_read(network_t* n, unsigned char* buf, int len, int timeout);
int network_write(network_t* n, unsigned char* buf, int len, int timeout);
int network_connect(network_t* n);
void network_disconnect(network_t *n);
void network_release(network_t* n);
int network_set_ca(network_t *n, const char *ca);
void network_set_channel(network_t *n, int channel);

#endif
