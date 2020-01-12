/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:02
 * @LastEditTime : 2020-01-12 11:09:25
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "mqtt_config.h"

typedef struct network_params {
    char                        *addr;
    char                        *port;
#if MQTT_NETWORK_TYPE_TLS
    const char                  *ca;
    unsigned int                ca_len;
    mbedtls_net_context         socket_fd;        /**< mbed TLS network context. */
    mbedtls_entropy_context     entropy;          /**< mbed TLS entropy. */
    mbedtls_ctr_drbg_context    ctr_drbg;         /**< mbed TLS ctr_drbg. */
    mbedtls_ssl_context         ssl;              /**< mbed TLS control context. */
    mbedtls_ssl_config          ssl_conf;         /**< mbed TLS configuration context. */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt            ca_cert;          /**< mbed TLS CA certification. */
    mbedtls_x509_crt            client_cert;      /**< mbed TLS Client certification. */
#endif
    mbedtls_pk_context          private_key;      /**< mbed TLS Client key. */
#endif /* MQTT_NETWORK_TYPE_TLS */
} network_params_t;

typedef struct network {
    int                     socket;
    network_params_t        *network_params;
    int                     (*connect)(struct network *);
    void                    (*disconnect)(struct network *);
    int                     (*read)(struct network *, unsigned char *, int, int);
    int                     (*write)(struct network *, unsigned char *, int, int);
} network_t;

int network_init(network_t* n);
int network_read(network_t* n, unsigned char* buf, int len, int timeout);
int network_write(network_t* n, unsigned char* buf, int len, int timeout);
int network_connect(network_t* n);
void network_release(network_t* n);

#endif
