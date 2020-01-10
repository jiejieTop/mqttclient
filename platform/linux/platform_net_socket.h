/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 13:39:00
 * @LastEditTime : 2020-01-11 01:30:44
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _PLATFORM_NET_SOCKET_H_
#define _PLATFORM_NET_SOCKET_H_
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

#define PLATFORM_ERR_NET_SOCKET_FAILED                     -0x0042  /**< Failed to open a socket. */
#define PLATFORM_ERR_NET_CONNECT_FAILED                    -0x0044  /**< The connection to the given server / port failed. */
#define PLATFORM_ERR_NET_BIND_FAILED                       -0x0046  /**< Binding of the socket failed. */
#define PLATFORM_ERR_NET_LISTEN_FAILED                     -0x0048  /**< Could not listen on the socket. */
#define PLATFORM_ERR_NET_ACCEPT_FAILED                     -0x004A  /**< Could not accept the incoming connection. */
#define PLATFORM_ERR_NET_RECV_FAILED                       -0x004C  /**< Reading information from the socket failed. */
#define PLATFORM_ERR_NET_SEND_FAILED                       -0x004E  /**< Sending information through the socket failed. */
#define PLATFORM_ERR_NET_CONN_RESET                        -0x0050  /**< Connection was reset by peer. */
#define PLATFORM_ERR_NET_UNKNOWN_HOST                      -0x0052  /**< Failed to get an IP address for the given hostname. */
#define PLATFORM_ERR_NET_BUFFER_TOO_SMALL                  -0x0043  /**< Buffer is too small to hold the data. */
#define PLATFORM_ERR_NET_INVALID_CONTEXT                   -0x0045  /**< The context is invalid, eg because it was free()ed. */
#define PLATFORM_ERR_NET_POLL_FAILED                       -0x0047  /**< Polling the net context failed. */
#define PLATFORM_ERR_NET_BAD_INPUT_DATA                    -0x0049  /**< Input invalid. */

#define PLATFORM_NET_PROTO_TCP  0 /**< The TCP transport protocol */
#define PLATFORM_NET_PROTO_UDP  1 /**< The UDP transport protocol */

int platform_net_socket_connect(const char *host, const char *port, int proto);
int platform_net_socket_recv(int fd, void *buf, size_t len, int flags);
int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout);
int platform_net_socket_write(int fd, void *buf, size_t len);
int platform_net_socket_write_timeout(int fd, unsigned char *buf, int len, int timeout);
int platform_net_socket_close(int fd);
int platform_net_socket_set_block(int fd);
int platform_net_socket_set_nonblock(int fd);
int platform_net_socket_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);

#endif /* _PLATFORM_NET_SOCKET_H_ */