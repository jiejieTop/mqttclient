/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime: 2019-12-15 15:12:30
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MQTTCLIENT_H_
#define _MQTTCLIENT_H_

#include <stdio.h>
#include <string.h>

#include "MQTTPacket.h"
#include "list.h"
#include "memory.h"
#include "network.h"
#include "error.h"

#define     MAX_PACKET_ID           65535
#define     MAX_MESSAGE_HANDLERS    5
#define     MQTT_TOPIC_LEN_MAX      128
#define     MQTT_REPUB_NUM_MAX      20
#define     MQTT_SUB_NUM_MAX        10

#define     DEFAULT_CMD_TIMEOUT     2000
#define     MAX_CMD_TIMEOUT         5000
#define     MIN_CMD_TIMEOUT         500


typedef enum qos {
    QOS0 = 0,
    QOS1 = 1,
    QOS2 = 2,
    SUBFAIL = 0x80
} qos_t;

typedef struct mqtt_message {
    qos_t               qos;
    unsigned char       retained;
    unsigned char       dup;
    unsigned short      id;
    void                *payload;
    size_t              payloadlen;
} mqtt_message_t;

typedef struct message_data {
    mqtt_message_t      *message;
    MQTTString          *topic_name;
} message_data_t;

typedef struct message_handlers {
    const char* topic_filter;
    void (*handler)(message_data_t*);
} message_handlers_t;

typedef struct mqtt_client {
    unsigned int        packet_id;
    unsigned int        cmd_timeout;
    size_t              read_buf_size;
    size_t              write_buf_size;
    unsigned char       *read_buf;
    unsigned char       *write_buf;
    void                *write_lock;
    list_t              msg_handlers;
    // message_handlers_t  msg_handlers[MAX_MESSAGE_HANDLERS];
    void                (*default_message_handler)(message_data_t*);
    network_t           *network;
    unsigned int        keep_alive_interval;
    char                connection_state;
    char                ping_outstanding;
    timer_t             ping_timer;
    list_t              ack_pend_list;
} mqtt_client_t;


typedef void (*disconnect_handler_t)(mqtt_client_t *, void *);
typedef void (*reconnect_handler_t)(mqtt_client_t *, void *);

typedef struct cli_init_params{
	unsigned int            cmd_timeout;
    size_t                  read_buf_size;
    size_t                  write_buf_size;
    connect_params_t        connect_params;
	disconnect_handler_t    disconnect_handler;
	reconnect_handler_t     reconnect_handler;
	void                    *disconnect_handler_data;
	void                    *reconnect_handler_data;
} cli_init_params_t;


int mqtt_init(mqtt_client_t *cli, cli_init_params_t *cli_init);



#endif /* _MQTTCLIENT_H_ */
