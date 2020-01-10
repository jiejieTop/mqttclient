/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:30:54
 * @LastEditTime : 2020-01-10 00:56:32
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_timer.h"
#include "platform_nettype_tcp.h"

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
	if ((NULL == n->network_params) || (NULL == n->network_params->addr) || (0 == n->network_params->port))
		RETURN_ERROR(MQTT_NULL_VALUE_ERROR);
	
	n->socket = -1;
    n->read = network_read;
	n->write = network_write;
	n->connect = network_connect;
	n->disconnect = network_disconnect;
	
	RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

void network_release(network_t* n)
{
	if (n->socket)
		platform_nettype_disconnect(n);

	memset(n, 0, sizeof(network_t));
}

