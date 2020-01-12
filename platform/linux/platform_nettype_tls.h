/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-11 19:45:44
 * @LastEditTime : 2020-01-12 00:53:42
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#ifndef _PLATFORM_NETTYPE_TLS_H_
#define _PLATFORM_NETTYPE_TLS_H_

#include "platform_net_socket.h"
#include "mqtt_config.h"
#include "network.h"
#include "error.h"
#include "log.h"

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

#endif