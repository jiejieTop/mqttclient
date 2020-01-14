/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-11 19:45:35
 * @LastEditTime : 2020-01-14 03:41:16
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_nettype_tls.h"
#include "platform_net_socket.h"
#include "platform_memory.h"
#include "random.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"

#if MQTT_NETWORK_TYPE_TLS

#ifndef AUTH_MODE_CERT
static const int ciphersuites[] = { MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA, MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA, 0 };
#endif

// static void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str)
// {
//     LOG_I("[mbedTLS]:[%s]:[%d]: %s", file, line, str);
// }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
static int server_certificate_verify(void *hostname, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    printf("%s:%d %s()..., flags = 0x%04x\n", __FILE__, __LINE__, __FUNCTION__, *flags);
    return *flags;
}
#endif

static int entropy_source(void *data, uint8_t *output, size_t len, size_t *olen)
{
    uint32_t seed;
    (void) data;
    seed = random_number();

    if (len > sizeof(seed)) {
        len = sizeof(seed);
    }

    memcpy(output, &seed, len);
    *olen = len;

    return 0;
}

static int platform_nettype_tls_init(network_t* n, nettype_tls_params_t* nettype_tls_params)
{
    int rc = MQTT_SUCCESS_ERROR;
    
    mbedtls_net_init(&(nettype_tls_params->socket_fd));
    mbedtls_ssl_init(&(nettype_tls_params->ssl));
    mbedtls_ssl_config_init(&(nettype_tls_params->ssl_conf));
    mbedtls_ctr_drbg_init(&(nettype_tls_params->ctr_drbg));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_init(&(nettype_tls_params->ca_cert));
    mbedtls_x509_crt_init(&(nettype_tls_params->client_cert));
    mbedtls_pk_init(&(nettype_tls_params->private_key));
#endif

    mbedtls_entropy_init(&(nettype_tls_params->entropy));
    mbedtls_entropy_add_source(&(nettype_tls_params->entropy), entropy_source, NULL, MBEDTLS_ENTROPY_MAX_GATHER, MBEDTLS_ENTROPY_SOURCE_STRONG);

    // custom parameter is NULL for now
    if ((rc = mbedtls_ctr_drbg_seed(&(nettype_tls_params->ctr_drbg), mbedtls_entropy_func,
                                    &(nettype_tls_params->entropy), NULL, 0)) != 0) {
        LOG_E("mbedtls_ctr_drbg_seed failed returned -0x%04x", -rc);
        RETURN_ERROR(PLATFORM_ERR_SSL_INIT);
    }
    
    LOG_D("Setting up the SSL/TLS structure...");
    if ((rc = mbedtls_ssl_config_defaults(&(nettype_tls_params->ssl_conf), MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        LOG_E("mbedtls_ssl_config_defaults failed returned -0x%04x", -rc);
        RETURN_ERROR(rc);
    }

    mbedtls_ssl_conf_rng(&(nettype_tls_params->ssl_conf), mbedtls_ctr_drbg_random, &(nettype_tls_params->ctr_drbg));

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (NULL != n->network_params.network_ssl_params.ca_crt) {
        n->network_params.network_ssl_params.ca_crt_len = strlen(n->network_params.network_ssl_params.ca_crt);
        LOG_I("ca len is %d", n->network_params.network_ssl_params.ca_crt_len);
        if ((rc = mbedtls_x509_crt_parse(&(nettype_tls_params->ca_cert), (const unsigned char *)n->network_params.network_ssl_params.ca_crt,
            (n->network_params.network_ssl_params.ca_crt_len + 1)))) {
            LOG_E("parse ca crt failed returned -0x%04x", -rc);
            RETURN_ERROR(PLATFORM_ERR_SSL_CERT);
        }
    }
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&(nettype_tls_params->ssl_conf), &(nettype_tls_params->ca_cert), NULL);
    if ((rc = mbedtls_ssl_conf_own_cert(&(nettype_tls_params->ssl_conf),
                                         &(nettype_tls_params->client_cert), &(nettype_tls_params->private_key))) != 0) {
        LOG_E("mbedtls_ssl_conf_own_cert failed returned 0x%04x", -rc);
        RETURN_ERROR(rc);
    }
    
    mbedtls_ssl_conf_verify(&(nettype_tls_params->ssl_conf), server_certificate_verify, (void *)n->network_params.addr);
    mbedtls_ssl_conf_authmode(&(nettype_tls_params->ssl_conf), MBEDTLS_SSL_VERIFY_REQUIRED);
#endif

#ifdef AUTH_MODE_CERT
    if (n->network_params.network_ssl_params.cert_file != NULL && n->network_params.network_ssl_params.key_file != NULL) {
            if ((rc = mbedtls_x509_crt_parse_file(&(nettype_tls_params->client_cert), n->network_params.network_ssl_params.cert_file)) != 0) {
            LOG_E("load client cert file failed returned 0x%x", rc<0?-rc:rc);
            return PLATFORM_ERR_SSL_CERT;
        }

        if ((rc = mbedtls_pk_parse_keyfile(&(nettype_tls_params->private_key), n->network_params.network_ssl_params.key_file, "")) != 0) {
            LOG_E("load client key file failed returned 0x%x", rc<0?-rc:rc);
            return PLATFORM_ERR_SSL_CERT;
        }
    } else {
        LOG_I("cert_file/key_file is empty! | cert_file = %s | key_file = %s", n->network_params.network_ssl_params.cert_file, n->network_params.network_ssl_params.key_file);
    }
#else
	if (n->network_params.network_ssl_params.psk != NULL && n->network_params.network_ssl_params.psk_id !=NULL) {
        const char *psk_id = n->network_params.network_ssl_params.psk_id;
        rc = mbedtls_ssl_conf_psk(&(nettype_tls_params->ssl_conf), (unsigned char *)n->network_params.network_ssl_params.psk, n->network_params.network_ssl_params.psk_length,
                                    (const unsigned char *) psk_id, strlen( psk_id ));
    } else {
        LOG_I("psk/pskid is empty! | psk = %s | psd_id = %s", n->network_params.network_ssl_params.psk, n->network_params.network_ssl_params.psk_id);
    }
	
	if (0 != rc) {
		LOG_E("mbedtls_ssl_conf_psk fail: 0x%x", (rc < 0 )? -rc : rc);
		return rc;
	}
#endif

    mbedtls_ssl_conf_read_timeout(&(nettype_tls_params->ssl_conf), n->network_params.network_ssl_params.timeout_ms);

#ifndef AUTH_MODE_CERT
    if(n->network_params.network_ssl_params.psk != NULL) {
        mbedtls_ssl_conf_ciphersuites(&(nettype_tls_params->ssl_conf), ciphersuites);
    }
#endif

    if ((rc = mbedtls_ssl_setup(&(nettype_tls_params->ssl), &(nettype_tls_params->ssl_conf))) != 0) {
        LOG_E("mbedtls_ssl_setup failed returned 0x%04x", -rc);
        RETURN_ERROR(rc);
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    // Set the hostname to check against the received server certificate and sni
    if ((rc = mbedtls_ssl_set_hostname(&(nettype_tls_params->ssl), n->network_params.addr)) != 0) {
        LOG_E("mbedtls_ssl_set_hostname failed returned 0x%04x", -rc);
        RETURN_ERROR(rc);
    }
#endif

    mbedtls_ssl_set_bio(&(nettype_tls_params->ssl), &(nettype_tls_params->socket_fd), mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}


int platform_nettype_tls_connect(network_t* n)
{
    int rc;
    if (NULL == n)
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);
    
    nettype_tls_params_t * nettype_tls_params = (nettype_tls_params_t *) platform_memory_alloc(sizeof(nettype_tls_params_t));

    if (NULL == nettype_tls_params)
        RETURN_ERROR(MQTT_MEM_NOT_ENOUGH_ERROR);


    rc = platform_nettype_tls_init(n, nettype_tls_params);
    if (MQTT_SUCCESS_ERROR != rc)
        goto exit;

    LOG_D("Connecting to /%s/%s...", n->network_params.addr, n->network_params.port);
    if (0 != (rc = mbedtls_net_connect(&(nettype_tls_params->socket_fd), n->network_params.addr, n->network_params.port, MBEDTLS_NET_PROTO_TCP)))
        goto exit;

    LOG_I("Performing the SSL/TLS handshake...");
    while ((rc = mbedtls_ssl_handshake(&(nettype_tls_params->ssl))) != 0) {
        if (rc != MBEDTLS_ERR_SSL_WANT_READ && rc != MBEDTLS_ERR_SSL_WANT_WRITE) {
            LOG_E("mbedtls_ssl_handshake failed returned 0x%04x", -rc);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
            if (rc == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
                LOG_E("Unable to verify the server's certificate");
            }
#endif
            goto exit;
        }
    }

    if ((rc = mbedtls_ssl_get_verify_result(&(nettype_tls_params->ssl))) != 0) {
        LOG_E("mbedtls_ssl_get_verify_result failed returned 0x%04x", -rc);
        goto exit;
    }

    // mbedtls_ssl_conf_read_timeout(&(nettype_tls_params->ssl_conf), 100);

    LOG_I("connected with /%s/%s...", n->network_params.addr, n->network_params.port);
    
    n->network_params.nettype_tls_params = nettype_tls_params;

    rc = MQTT_SUCCESS_ERROR;

exit:
    RETURN_ERROR(rc);
}


#endif /* MQTT_NETWORK_TYPE_TLS */