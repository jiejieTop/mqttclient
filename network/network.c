/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:30:54
 * @LastEditTime: 2019-12-16 01:59:39
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "nettype.h"

int network_read(network_t *n, unsigned char *buf, int len, int timeout)
{
	return platform_nettype_read(n, buf, len, timeout);
}

int network_write(network_t *n, unsigned char *buf, int len, int timeout)
{
	return platform_nettype_write(n, buf, len, timeout);
}

int network_connect(network_t *n)
{
	return platform_nettype_connect(n);
}
void network_disconnect(network_t *n)
{
	platform_nettype_disconnect(n);
}

void network_init(network_t *n, connect_params_t *connect_params)
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

void network_release(network_t* n)
{
	if (n->socket)
		platform_nettype_disconnect(n);

	memset(n, 0, sizeof(network_t));
}

