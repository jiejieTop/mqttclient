/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 13:38:52
 * @LastEditTime : 2019-12-30 22:13:06
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "nettype.h"
#include "log.h"

int platform_nettype_read(network_t *n, unsigned char *read_buf, int len, int timeout)
{
    int rc;
    int bytes = 0;
	struct timeval tv = {
        timeout / 1000, 
        (timeout % 1000) * 1000
    };
    
	if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec <= 0)) {
		tv.tv_sec = 0;
		tv.tv_usec = 100;
	}

	setsockopt(n->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	while (bytes < len) {
		rc = recv(n->socket, &read_buf[bytes], (size_t)(len - bytes), 0);
		if (rc == -1) {
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			  bytes = -1;
			break;
		} else if (rc == 0)	{
			bytes = 0;
			break;
		} else
			bytes += rc;
	}
	return bytes;
}


int platform_nettype_write(network_t *n, unsigned char *write_buf, int len, int timeout)
{
	struct timeval tv = {
        timeout / 1000, 
        (timeout % 1000) * 1000
    };
    
	if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec <= 0)) {
		tv.tv_sec = 0;
		tv.tv_usec = 100;
	}

	setsockopt(n->socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
	
    return write(n->socket, write_buf, len);
}


int platform_nettype_connect(network_t* n)
{
    int ret;
    struct hostent *he;
    struct in_addr addr;
    struct sockaddr_in server;
    char addr_str[INET_ADDRSTRLEN];

    ret = inet_pton(AF_INET, n->network_params->addr, &addr);
    if (ret == 0) {
        LOG_I("get %s ip addr...", n->network_params->addr);
        if ((he = gethostbyname(n->network_params->addr)) == NULL) {
            LOG_E("get host ip addr error.");
            RETURN_ERROR(CONNECT_FAIL_ERROR);
        } else {
            addr = *((struct in_addr *)he->h_addr);
            if(inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str)) != NULL)
                LOG_I("host name: %s, ip addr:%s",n->network_params->addr, addr_str);
        }
    } else if(ret == 1) {
        if(inet_ntop(AF_INET, &addr, addr_str, sizeof(addr_str)) != NULL)
            LOG_I("host addr: %s", addr_str);
    } else {
        LOG_E("inet_pton function return %d", ret);
    }
    
    if (-1 == n->socket) {
        if ((n->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            LOG_E("create an endpoint for communication fail!");
            RETURN_ERROR(CONNECT_FAIL_ERROR);
        }
    }

    bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(n->network_params->port);
	server.sin_addr = addr;

    if (connect(n->socket, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {
        LOG_E("connect server fail..., n->socket = %d", n->socket);
        close(n->socket);
        n->socket = -1;
        RETURN_ERROR(CONNECT_FAIL_ERROR);
    }

    LOG_I("connect server success..., n->socket = %d", n->socket);

    RETURN_ERROR(SUCCESS_ERROR);
}


void platform_nettype_disconnect(network_t* n)
{
    if (NULL != n)
	    close(n->socket);
    n->socket = -1;
}
