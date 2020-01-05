/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime : 2020-01-05 13:36:05
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
#include "thread.h"
#include "error.h"
#include "mutex.h"
#include "log.h"

#define     MQTT_MAX_PACKET_ID                  65535
#define     MQTT_TOPIC_LEN_MAX                  64
#define     MQTT_ACK_HANDLER_NUM_MAX            64
#define     MQTT_DEFAULT_BUF_SIZE               1024
#define     MQTT_DEFAULT_CMD_TIMEOUT            4000
#define     MQTT_MAX_CMD_TIMEOUT                20000
#define     MQTT_MIN_CMD_TIMEOUT                1000
#define     MQTT_KEEP_ALIVE_INTERVAL            20      // unit: second
#define     MQTT_VERSION                        4       // 4 is mqtt 3.1.1
#define     MQTT_RECONNECT_DEFAULT_DURATION     1000
#define     MQTT_THREAD_STACK_SIZE              2048
#define     MQTT_THREAD_PRIO                    10
#define     MQTT_THREAD_TICK                    50

typedef enum mqtt_qos {
    QOS0 = 0,
    QOS1 = 1,
    QOS2 = 2,
    SUBFAIL = 0x80
} mqtt_qos_t;

typedef enum client_state {
	CLIENT_STATE_INVALID = -1,
	CLIENT_STATE_INITIALIZED = 0,
	CLIENT_STATE_CONNECTED = 1,
	CLIENT_STATE_DISCONNECTED = 2,
}client_state_t;

typedef struct mqtt_connack_data {
    unsigned char rc;
    unsigned char session_present;
} mqtt_connack_data_t;

typedef struct mqtt_message {
    mqtt_qos_t          qos;
    unsigned char       retained;
    unsigned char       dup;
    unsigned short      id;
    size_t              payloadlen;
    void                *payload;
} mqtt_message_t;

typedef struct message_data {
    char                topic_name[MQTT_TOPIC_LEN_MAX];
    mqtt_message_t      *message;
} message_data_t;

typedef void (*message_handler_t)(void* client, message_data_t* msg);
typedef void (*reconnect_handler_t)(void* client, void* reconnect_date);

typedef struct message_handlers {
    list_t              list;
    mqtt_qos_t          qos;
    const char*         topic_filter;
    message_handler_t   handler;
} message_handlers_t;

typedef struct ack_handlers {
    list_t              list;
    platform_timer_t    timer;
    unsigned int        type;
    unsigned short      packet_id;
    message_handlers_t  *handler;
    unsigned short      payload_len;
    unsigned char       *payload;
} ack_handlers_t;

typedef struct connect_params {
    char                *client_id;
    char                *user_name;
    char                *password;
    size_t              client_id_len;
    size_t              user_name_len;
    size_t              password_len;
	unsigned char       will_flag;
    void                *will_options;
    unsigned short      keep_alive_interval;
    unsigned char       clean_session;
    unsigned char       mqtt_version;
    network_params_t    network_params;
} connect_params_t;

typedef struct mqtt_client {
    unsigned short              packet_id;
    unsigned char               *read_buf;
    unsigned char               *write_buf;
    unsigned char               ping_outstanding;
    unsigned char               ack_handler_number;
    unsigned int                cmd_timeout;
    unsigned int                read_buf_size;
    unsigned int                write_buf_size;
    unsigned int                reconnect_try_duration;
    void                        *reconnect_date;
    reconnect_handler_t         reconnect_handler;
    client_state_t              client_state;
    platform_mutex_t            write_lock;
    platform_mutex_t            global_lock;
    list_t                      msg_handler_list;
    list_t                      ack_handler_list;
    network_t                   *network;
    platform_thread_t           *thread;
    platform_timer_t            reconnect_timer;
    platform_timer_t            last_sent;
    platform_timer_t            last_received;
    connect_params_t            *connect_params;
    // void                        (*default_message_handler)(void*, message_data_t*);
} mqtt_client_t;

typedef struct client_init_params{
	unsigned int                cmd_timeout;
    unsigned int                read_buf_size;
    unsigned int                write_buf_size;
    unsigned int                reconnect_try_duration;
    void                        *reconnect_date;
    reconnect_handler_t         reconnect_handler;
    connect_params_t            connect_params;
} client_init_params_t;

int mqtt_keep_alive(mqtt_client_t* c);
int mqtt_init(mqtt_client_t* c, client_init_params_t* init);
int mqtt_release(mqtt_client_t* c);
int mqtt_connect(mqtt_client_t* c);
int mqtt_disconnect(mqtt_client_t* c);
int mqtt_subscribe(mqtt_client_t* c, const char* topic_filter, mqtt_qos_t qos, message_handler_t msg_handler);
int mqtt_unsubscribe(mqtt_client_t* c, const char* topic_filter);
int mqtt_publish(mqtt_client_t* c, const char* topic_filter, mqtt_message_t* msg);
int mqtt_yield(mqtt_client_t* c, int timeout_ms);



#endif /* _MQTTCLIENT_H_ */
