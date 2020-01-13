/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:30:54
 * @LastEditTime : 2020-01-13 09:23:21
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_timer.h"
#include "platform_nettype_tcp.h"

#if MQTT_NETWORK_TYPE_TLS
#include "platform_nettype_tls.h"
#endif

int network_read(network_t *n, unsigned char *buf, int len, int timeout)
{
	return platform_nettype_tcp_read(n, buf, len, timeout);
}

int network_write(network_t *n, unsigned char *buf, int len, int timeout)
{
	return platform_nettype_tcp_write(n, buf, len, timeout);
}

int network_connect(network_t *n)
{
#if MQTT_NETWORK_TYPE_TLS
	return platform_nettype_tls_connect(n);
#else
	return platform_nettype_tcp_connect(n);
#endif
}

void network_disconnect(network_t *n)
{
	platform_nettype_tcp_disconnect(n);
}

int network_init(network_t* n, network_params_t* network_params)
{
	if ((NULL == n) || (NULL == network_params) || (NULL == network_params->addr) || (NULL == network_params->port))
		RETURN_ERROR(MQTT_NULL_VALUE_ERROR);
	
	n->socket = -1;
    n->read = network_read;
	n->write = network_write;
	n->connect = network_connect;
	n->disconnect = network_disconnect;
	n->network_params.addr = network_params->addr;
	n->network_params.port = network_params->port;
#if MQTT_NETWORK_TYPE_TLS
	n->network_params.network_tls_params.ca_crt = network_params->network_tls_params.ca_crt;
	n->network_params.network_tls_params.ca_crt_len = strlen(n->network_params.network_tls_params.ca_crt);
	LOG_D("n->network_params.network_tls_params.ca_crt_len = %d", n->network_params.network_tls_params.ca_crt_len);
	if (0 == network_params->network_tls_params.timeout_ms)
		network_params->network_tls_params.timeout_ms = MQTT_TLS_HANDSHAKE_TIMEOUT;
	n->network_params.network_tls_params.timeout_ms = network_params->network_tls_params.timeout_ms;
#endif

	RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

void network_release(network_t* n)
{
	if (n->socket)
		platform_nettype_tcp_disconnect(n);

	memset(n, 0, sizeof(network_t));
}

