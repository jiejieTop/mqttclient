/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:30:54
 * @LastEditTime : 2019-12-19 21:44:09
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

int network_init(network_t *n)
{
    n->socket = -1;
	if ((NULL == n->connect_params) || (NULL == n->connect_params->addr) || (0 == n->connect_params->port))
		RETURN_ERROR(NULL_VALUE_ERROR);
	
    n->read = network_read;
	n->write = network_write;
	n->connect = network_connect;
	n->disconnect = network_disconnect;
	
	RETURN_ERROR(SUCCESS_ERROR);
}

void network_release(network_t* n)
{
	if (n->socket)
		platform_nettype_disconnect(n);

	memset(n, 0, sizeof(network_t));
}

