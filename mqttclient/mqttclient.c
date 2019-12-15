/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime: 2019-12-16 02:30:22
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "mqttclient.h"

static void default_msg_handler(void* client, message_data_t* msg)
{
    (void*) client;
    printf("%s:%d %s()...\ntopic: %s\nmessage:%s\n", 
            __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name->cstring, (char*)msg->message->payload);
}

client_state_t mqtt_get_client_state(mqtt_client_t* c)
{
    return c->client_state;
}

int mqtt_send_packet()
{
    
}

int waitfor(mqtt_client_t* c, int packet_type, platform_timer_t* timer)
{

}

int mqtt_connect_with_results(mqtt_client_t* c, MQTTPacket_connectData* connect_data, mqtt_connack_data_t* connack_data)
{
    platform_timer_t connect_timer;
    int len = 0;
    int rc = FAIL_ERROR;
    MQTTPacket_connectData default_connect_data = MQTTPacket_connectData_initializer;

    if (NULL == connect_data)
        connect_data = &default_connect_data; /* set default connect data if none were supplied */

    if ((NULL == c) || (NULL == connack_data)) 
        RETURN_ERROR(NULL_VALUE_ERROR);

    if (CLIENT_STATE_CONNECTED == mqtt_get_client_state(c))
        RETURN_ERROR(SUCCESS_ERROR);

    platform_timer_init(&connect_timer);
    platform_timer_cutdown(&connect_timer, c->cmd_timeout);

    rc = c->network->connect(c->network);
    if (SUCCESS_ERROR != rc)
        RETURN_ERROR(rc);
    
    c->keep_alive_interval = connect_data->keepAliveInterval;
    c->clean_session = connect_data->cleansession;
    platform_timer_cutdown(&c->last_received, c->keep_alive_interval);

    platform_mutex_lock(&c->write_lock);

    if ((len = MQTTSerialize_connect(c->write_buf, c->write_buf_size, connect_data)) <= 0)
        goto exit;
    if ((rc = mqtt_send_packet(c, len, &connect_timer)) != SUCCESS_ERROR)  // send the connect packet
        goto exit; // there was a problem

    // this will be a blocking call, wait for the connack
    if (waitfor(c, CONNACK, &connect_timer) == CONNACK)
    {
        connack_data->rc = 0;
        connack_data->session_present = 0;
        if (MQTTDeserialize_connack(&connack_data->session_present, &connack_data->rc, c->read_buf, c->read_buf_size) == 1)
            rc = connack_data->rc;
        else
            rc = FAIL_ERROR;
    }
    else
        rc = FAIL_ERROR;

exit:
    if (rc == SUCCESS_ERROR)
    {
        c->client_state = CLIENT_STATE_CONNECTED;
        c->ping_outstanding = 0;
    }
    
    platform_mutex_unlock(&c->write_lock);
    
}

int mqtt_init(mqtt_client_t* c, client_init_params_t* init)
{
    if ((NULL == c) || (NULL == init)) 
        RETURN_ERROR(NULL_VALUE_ERROR);

    memset(c, 0, sizeof(mqtt_client_t));

    /* network init */
    c->network = (network_t*) memory_malloc(sizeof(network_t));
    if (NULL == c->network) {
        printf("malloc network failed...\n");
        RETURN_ERROR(MEM_NOT_ENOUGH_ERROR);
    }
    memset(c->network, 0, sizeof(network_t));

    network_init(c->network, &init->connect_params);

    if (0 == init->read_buf_size)
        init->read_buf_size = DEFAULT_BUF_SIZE;
    if (0 == init->write_buf_size)
        init->write_buf_size = DEFAULT_BUF_SIZE;
    
    c->read_buf = (char*) memory_malloc(init->read_buf_size);
    c->write_buf = (char*) memory_malloc(init->write_buf_size);
    if ((NULL == c->read_buf) || (NULL == c->write_buf)){
        printf("malloc read buffer failed...\n");
        RETURN_ERROR(MEM_NOT_ENOUGH_ERROR);
    }

    c->packet_id = 1;
    if ((init->cmd_timeout < MIN_CMD_TIMEOUT) || (init->cmd_timeout > MAX_CMD_TIMEOUT))
        c->cmd_timeout = DEFAULT_CMD_TIMEOUT;
    else
        c->cmd_timeout = init->cmd_timeout;
    
    c->clean_session = 0;
    c->connection_state = 0;
    c->ping_outstanding = 0;
    c->connect_data = init->connect_params;
    c->default_message_handler = default_msg_handler;

    list_init(&c->msg_handler_list);
    platform_mutex_init(&c->write_lock);
    platform_timer_init(&c->ping_timer);
    platform_timer_init(&c->last_sent);
    platform_timer_init(&c->last_received);

    RETURN_ERROR(SUCCESS_ERROR);
}

int mqtt_release(mqtt_client_t* c)
{
    if (NULL == c)
        RETURN_ERROR(NULL_VALUE_ERROR);
    
    if (NULL != c->network) {
        network_release(c->network);
        memory_free(c->network);
        c->network = NULL;
    }

    if (NULL != c->read_buf) {
        memory_free(c->read_buf);
        c->read_buf = NULL;
    }

    if (NULL != c->read_buf) {
        memory_free(c->read_buf);
        c->read_buf = NULL;
    }

    if (!(list_empty(&c->msg_handler_list)))
        list_del_init(&c->msg_handler_list);
    
    memset(c, 0, sizeof(mqtt_client_t));

    RETURN_ERROR(SUCCESS_ERROR);
}

int mqtt_connect(mqtt_client_t* c, MQTTPacket_connectData* connect_data)
{
    mqtt_connack_data_t connack_data;
    mqtt_connect_with_results(c, connect_data, &connack_data);
}

