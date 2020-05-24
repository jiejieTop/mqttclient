/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:30:54
 * @LastEditTime: 2020-05-24 17:11:35
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_timer.h"
#include "platform_memory.h"
#include "nettype_tcp.h"
#include "nettype_tls.h"

int network_read(network_t *n, unsigned char *buf, int len, int timeout)
{
    if (n->channel)
        return nettype_tls_read(n, buf, len, timeout);
    
    return nettype_tcp_read(n, buf, len, timeout);
}

int network_write(network_t *n, unsigned char *buf, int len, int timeout)
{
    if (n->channel)
        return nettype_tls_write(n, buf, len, timeout);

    return nettype_tcp_write(n, buf, len, timeout);
}

int network_connect(network_t *n)
{
    if (n->channel)
        return nettype_tls_connect(n);
    
    return nettype_tcp_connect(n);

}

void network_disconnect(network_t *n)
{
    if (n->channel)
        nettype_tls_disconnect(n);
    else
        nettype_tcp_disconnect(n);

}

int network_init(network_t *n, const char *host, const char *port, const char *ca)
{
    if (NULL == n)
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);

    n->socket = -1;
    n->host = host;
    n->port = port;
    n->channel = 0;

    if (NULL != ca) {
        network_set_ca(n, ca);
    }

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

void network_release(network_t* n)
{
    if (n->socket >= 0)
        network_disconnect(n);

    memset(n, 0, sizeof(network_t));
}

void network_set_channel(network_t *n, int channel)
{
    n->channel = channel;
}

int network_set_ca(network_t *n, const char *ca)
{
    if ((NULL == n) || (NULL == ca))
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);
    
    n->ca_crt = ca;
    n->ca_crt_len = strlen(ca);
    n->channel = NETWORK_CHANNEL_TLS;
    n->timeout_ms = MQTT_TLS_HANDSHAKE_TIMEOUT;

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}


int network_set_host_port(network_t* n, char *host, char *port)
{
    if (!(n && host && port))
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);

    n->host = host;
    n->port = port;
    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}

