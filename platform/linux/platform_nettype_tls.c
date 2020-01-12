/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-11 19:45:35
 * @LastEditTime : 2020-01-12 20:43:30
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_nettype_tls.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "platform_net_socket.h"

static void mbedtls_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    LOG_I("[mbedTLS]:[%s]:[%d]: %s", file, line, str);
}

#if defined(MBEDTLS_X509_CRT_PARSE_C)
static int server_certificate_verify(void *hostname, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    return *flags;
}
#endif

static int platform_nettype_tls_init(network_t* n)
{
    int rc = MQTT_SUCCESS_ERROR;
    
    if (NULL == n->network_params)
        RETURN_ERROR(MQTT_NULL_VALUE_ERROR);

    mbedtls_net_init(&n->network_params->socket_fd);
    mbedtls_ssl_init(&n->network_params->ssl);
    mbedtls_ssl_config_init(&n->network_params->ssl_conf);
    mbedtls_ctr_drbg_init(&n->network_params->ctr_drbg);
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_init(&n->network_params->ca_cert);
    mbedtls_x509_crt_init(&n->network_params->client_cert);
    mbedtls_pk_init(&n->network_params->private_key);
#endif
    mbedtls_entropy_init(&n->network_params->entropy);
    mbedtls_ssl_conf_dbg(&n->network_params->ssl_conf, mbedtls_debug, NULL);

    if ((rc = mbedtls_ctr_drbg_seed(&n->network_params->ctr_drbg, mbedtls_entropy_func,
                                        &n->network_params->entropy, NULL, 0)) != 0) {
        LOG_E("mbedtls_ctr_drbg_seed failed returned -0x%04x", -rc);
        RETURN_ERROR(PLATFORM_ERR_SSL_INIT);
    }
    
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if (n->network_params->ca != NULL) {
        n->network_params->ca_len = strlen(n->network_params->ca);
        LOG_I("ca len is %d", n->network_params->ca_len);
        if ((rc = mbedtls_x509_crt_parse(&n->network_params->ca_cert, (const unsigned char *)n->network_params->ca,
            (n->network_params->ca_len + 1)))) {
            LOG_E("parse ca crt failed returned -0x%04x", -rc);
            RETURN_ERROR(PLATFORM_ERR_SSL_CERT);
        }
    }
#endif

    RETURN_ERROR(MQTT_SUCCESS_ERROR);
}


int platform_nettype_tls_connect(network_t* n)
{
    int rc;
    rc = platform_nettype_tls_init(n);
    if (MQTT_SUCCESS_ERROR != rc)
        goto exit;
    
    if (0 != (rc = mbedtls_net_connect(&n->network_params->socket_fd, n->network_params->addr, n->network_params->port, MBEDTLS_NET_PROTO_TCP)))
        goto exit;
    
    if ((rc = mbedtls_ssl_config_defaults(&n->network_params->ssl_conf, MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0)  {
        LOG_E("mbedtls_ssl_config_defaults failed returned -0x%04x", -rc);
        goto exit;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_verify(&n->network_params->ssl_conf, server_certificate_verify, (void *)n->network_params->addr);
    mbedtls_ssl_conf_authmode(&n->network_params->ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#endif
    mbedtls_ssl_conf_rng(&n->network_params->ssl_conf, mbedtls_ctr_drbg_random, &n->network_params->ctr_drbg);

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&n->network_params->ssl_conf, &n->network_params->ca_cert, NULL);
    if ((rc = mbedtls_ssl_conf_own_cert(&n->network_params->ssl_conf,
                                         &n->network_params->client_cert, &n->network_params->private_key)) != 0) {
        LOG_E("mbedtls_ssl_conf_own_cert failed returned 0x%04x", -rc);
        goto exit;
    }
#endif
    
    mbedtls_ssl_conf_read_timeout(&n->network_params->ssl_conf, TLS_HANDSHAKE_TIMEOUT);
    if ((rc = mbedtls_ssl_setup(&n->network_params->ssl, &n->network_params->ssl_conf)) != 0) {
        LOG_E("mbedtls_ssl_setup failed returned 0x%04x", -rc);
        goto exit;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    // Set the hostname to check against the received server certificate and sni
    if ((rc = mbedtls_ssl_set_hostname(&n->network_params->ssl, n->network_params->addr)) != 0) {
        LOG_E("mbedtls_ssl_set_hostname failed returned 0x%04x", -rc);
        goto exit;
    }
#endif

    mbedtls_ssl_set_bio(&n->network_params->ssl, &n->network_params->socket_fd, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    LOG_I("Performing the SSL/TLS handshake...");
    while ((rc = mbedtls_ssl_handshake(&n->network_params->ssl)) != 0) {
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

    if ((rc = mbedtls_ssl_get_verify_result(&n->network_params->ssl)) != 0) {
        LOG_E("mbedtls_ssl_get_verify_result failed returned 0x%04x", -rc);
        goto exit;
    }

    mbedtls_ssl_conf_read_timeout(&n->network_params->ssl_conf, 100);

    rc = MQTT_SUCCESS_ERROR;

exit:
    RETURN_ERROR(rc);
}
