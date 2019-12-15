/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:30:54
 * @LastEditTime: 2019-12-15 15:44:59
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "nettype.h"

int network_read(network_t *n, unsigned char *buf, int len, int timeout)
{
	return nettype_read(n, buf, len, timeout);
}

int network_write(network_t *n, unsigned char *buf, int len, int timeout)
{
	return nettype_write(n, buf, len, timeout);
}

int network_connect(network_t *n)
{
	return nettype_connect(n);
}
void network_disconnect(network_t *n)
{
	nettype_disconnect(n);
}

int network_init(network_t *n, connect_params_t *connect_params)
{
    n->socket = -1;
    n->connect_params.addr = connect_params->addr;
    n->connect_params.port = connect_params->port;
    n->connect_params.ca = connect_params->ca;

    n->read = network_read;
	n->write = network_write;
	n->connect = network_connect;
	n->disconnect = network_disconnect;
}