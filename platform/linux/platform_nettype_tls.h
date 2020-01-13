/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-11 19:45:44
 * @LastEditTime : 2020-01-13 07:26:26
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#ifndef _PLATFORM_NETTYPE_TLS_H_
#define _PLATFORM_NETTYPE_TLS_H_

#include "platform_net_socket.h"
#include "mqtt_config.h"
#include "network.h"
#include "error.h"
#include "log.h"

#if MQTT_NETWORK_TYPE_TLS

typedef struct nettype_tls_params {
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
} nettype_tls_params_t;

#define     PLATFORM_ERR_SSL_INIT                                      -701;    // 表示SSL初始化失败
#define     PLATFORM_ERR_SSL_CERT                                      -702;    // 表示SSL证书相关问题
#define     PLATFORM_ERR_SSL_CONNECT                                   -703;    // 表示SSL连接失败
#define     PLATFORM_ERR_SSL_CONNECT_TIMEOUT                           -704;    // 表示SSL连接超时
#define     PLATFORM_ERR_SSL_WRITE_TIMEOUT                             -705;    // 表示SSL写超时
#define     PLATFORM_ERR_SSL_WRITE                                     -706;    // 表示SSL写错误
#define     PLATFORM_ERR_SSL_READ_TIMEOUT                              -707;    // 表示SSL读超时
#define     PLATFORM_ERR_SSL_READ                                      -708;    // 表示SSL读错误
#define     PLATFORM_ERR_SSL_NOTHING_TO_READ                           -709;    // 表示底层没有数据可以读取

int platform_nettype_tls_read(network_t *n, unsigned char *buf, int len, int timeout);
int platform_nettype_tls_write(network_t *n, unsigned char *buf, int len, int timeout);
int platform_nettype_tls_connect(network_t* n);
void platform_nettype_tls_disconnect(network_t* n);

#endif /* MQTT_NETWORK_TYPE_TLS */

#endif
