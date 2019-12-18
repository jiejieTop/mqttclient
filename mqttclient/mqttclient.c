/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime : 2019-12-18 21:22:57
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "mqttclient.h"

static int mqtt_decode_packet(mqtt_client_t* c, int* value, int timeout);
static int mqtt_read_packet(mqtt_client_t* c, platform_timer_t* timer);
int mqtt_send_packet(mqtt_client_t* c, int length, platform_timer_t* timer);

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

static void mqtt_new_message_data(message_data_t* md, MQTTString* topic_name, mqtt_message_t* message) {
    md->topic_name = topic_name;
    md->message = message;
}

static char isTopicMatched(char* topicFilter, MQTTString* topicName)
{
    char* curf = topicFilter;
    char* curn = topicName->lenstring.data;
    char* curn_end = curn + topicName->lenstring.len;

    while (*curf && curn < curn_end)
    {
        if (*curn == '/' && *curf != '/')
            break;
        if (*curf != '+' && *curf != '#' && *curf != *curn)
            break;
        if (*curf == '+')
        {   // skip until we meet the next separator, or end of string
            char* nextpos = curn + 1;
            while (nextpos < curn_end && *nextpos != '/')
                nextpos = ++curn + 1;
        }
        else if (*curf == '#')
            curn = curn_end - 1;    // skip until end of string
        curf++;
        curn++;
    };

    return (curn == curn_end) && (*curf == '\0');
}

int mqtt_deliver_message(mqtt_client_t* c, MQTTString* topic_name, mqtt_message_t* message)
{
    int i;
    int rc = FAIL_ERROR;
    list_t *curr, *next;
    message_handlers_t *msg_handler;
    
    LIST_FOR_EACH_SAFE(curr, next, &c->msg_handler_list) {
        msg_handler = LIST_ENTRY(curr, message_handlers_t, list);

        if ((0 != msg_handler->topic_filter) && ((MQTTPacket_equals(topic_name, (char*)msg_handler->topic_filter)) || 
            (isTopicMatched((char*)msg_handler->topic_filter, topic_name)))) {
            
            if (NULL != msg_handler->handler) {
                message_data_t md;
                mqtt_new_message_data(&md, topic_name, message);
                msg_handler->handler(c, &md);
                rc = SUCCESS_ERROR;
            }
        }

    }

    if (rc == FAIL_ERROR && c->default_message_handler != NULL)
    {
        message_data_t md;
        mqtt_new_message_data(&md, topic_name, message);
        c->default_message_handler(c, &md);
        rc = SUCCESS_ERROR;
    }

    return rc;
}

void mqtt_clean_session(mqtt_client_t* c)
{

    list_t *curr, *next;
    message_handlers_t *msg_handler;
    
    LIST_FOR_EACH_SAFE(curr, next, &c->msg_handler_list) {
        msg_handler = LIST_ENTRY(curr, message_handlers_t, list);
        msg_handler->topic_filter = NULL;
    }
}

void mqtt_close_session(mqtt_client_t* c)
{
    c->ping_outstanding = 0;
    c->client_state = CLIENT_STATE_INITIALIZED;
    if (c->connect_data->clean_session)
        mqtt_clean_session(c);
}

int keepalive(mqtt_client_t* c)
{
    int rc = SUCCESS_ERROR;

    if (c->connect_data->keep_alive_interval == 0)
        goto exit;

    if (platform_timer_is_expired(&c->last_sent) || platform_timer_is_expired(&c->last_received))
    {
        if (c->ping_outstanding)
            rc = FAIL_ERROR; /* PINGRESP not received in keepalive interval */
        else
        {
            platform_timer_t timer;
            platform_timer_init(&timer);
            platform_timer_cutdown(&timer, 1000);
            int len = MQTTSerialize_pingreq(c->write_buf, c->write_buf_size);
            if (len > 0 && (rc = mqtt_send_packet(c, len, &timer)) == SUCCESS_ERROR) // send the ping packet
                c->ping_outstanding = 1;
        }
    }

exit:
    return rc;
}


int cycle(mqtt_client_t* c, platform_timer_t* timer)
{
    int len = 0,
        rc = SUCCESS_ERROR;
    printf("mqtt_read_packet\n");
    int packet_type = mqtt_read_packet(c, timer);     /* read the socket, see what work is due */
    printf("packet_type is %d\n",packet_type);
    switch (packet_type)
    {
        default:
            /* no more data to read, unrecoverable. Or read packet fails due to unexpected network error */
            rc = packet_type;
            goto exit;
        case 0: /* timed out reading packet */
            break;
        case CONNACK:
        case PUBACK:
        case SUBACK:
        case UNSUBACK:
            break;
        case PUBLISH:
        {
            MQTTString topic_name;
            mqtt_message_t msg;
            int qos;
            msg.payloadlen = 0; /* this is a size_t, but deserialize publish sets this as int */
            if (MQTTDeserialize_publish(&msg.dup, &qos, &msg.retained, &msg.id, &topic_name,
               (unsigned char**)&msg.payload, (int*)&msg.payloadlen, c->read_buf, c->read_buf_size) != 1)
                goto exit;
            msg.qos = (mqtt_qos_t)qos;
            mqtt_deliver_message(c, &topic_name, &msg);
            if (msg.qos != QOS0)
            {
                if (msg.qos == QOS1)
                    len = MQTTSerialize_ack(c->write_buf, c->write_buf_size, PUBACK, 0, msg.id);
                else if (msg.qos == QOS2)
                    len = MQTTSerialize_ack(c->write_buf, c->write_buf_size, PUBREC, 0, msg.id);
                if (len <= 0)
                    rc = FAIL_ERROR;
                else
                    rc = mqtt_send_packet(c, len, timer);
                if (rc == FAIL_ERROR)
                    goto exit; // there was a problem
            }
            break;
        }
        case PUBREC:
        case PUBREL:
        {
            unsigned short mypacketid;
            unsigned char dup, type;
            if (MQTTDeserialize_ack(&type, &dup, &mypacketid, c->read_buf, c->read_buf_size) != 1)
                rc = FAIL_ERROR;
            else if ((len = MQTTSerialize_ack(c->write_buf, c->write_buf_size,
                (packet_type == PUBREC) ? PUBREL : PUBCOMP, 0, mypacketid)) <= 0)
                rc = FAIL_ERROR;
            else if ((rc = mqtt_send_packet(c, len, timer)) != SUCCESS_ERROR) // send the PUBREL packet
                rc = FAIL_ERROR; // there was a problem
            if (rc == FAIL_ERROR)
                goto exit; // there was a problem
            break;
        }

        case PUBCOMP:
            break;
        case PINGRESP:
            c->ping_outstanding = 0;
            break;
    }

    if (keepalive(c) != SUCCESS_ERROR) {
        //check only keepalive FAIL_ERROR status so that previous FAIL_ERROR status can be considered as FAULT
        rc = FAIL_ERROR;
    }

exit:
    if (rc == SUCCESS_ERROR)
        rc = packet_type;
    else if (c->connection_state)
        mqtt_close_session(c);
    return rc;
}

static int mqtt_decode_packet(mqtt_client_t* c, int* value, int timeout)
{
    unsigned char i;
    int multiplier = 1;
    int len = 0;
    const int MAX_NO_OF_REMAINING_LENGTH_BYTES = 4;

    *value = 0;
    do
    {
        int rc = MQTTPACKET_READ_ERROR;

        if (++len > MAX_NO_OF_REMAINING_LENGTH_BYTES)
        {
            rc = MQTTPACKET_READ_ERROR; /* bad data */
            goto exit;
        }
        rc = c->network->read(c->network, &i, 1, timeout);
        if (rc != 1)
            goto exit;
        *value += (i & 127) * multiplier;
        multiplier *= 128;
    } while ((i & 128) != 0);
exit:
    return len;
}

static int mqtt_read_packet(mqtt_client_t* c, platform_timer_t* timer)
{
    MQTTHeader header = {0};
    int len = 0;
    int rem_len = 0;

    /* 1. read the header byte.  This has the packet type in it */
    int rc = c->network->read(c->network, c->read_buf, 1, platform_timer_timer_remain(timer));
    if (rc != 1)
        goto exit;

    len = 1;
    /* 2. read the remaining length.  This is variable in itself */
    mqtt_decode_packet(c, &rem_len, platform_timer_timer_remain(timer));

    /* put the original remaining length back into the buffer */
    len += MQTTPacket_encode(c->read_buf + 1, rem_len); 

    if (rem_len > (c->read_buf_size - len)) {
        rc = BUFFER_OVERFLOW_ERROR;
        goto exit;
    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if (rem_len > 0 && (rc = c->network->read(c->network, c->read_buf + len, rem_len, platform_timer_timer_remain(timer)) != rem_len)) {
        rc = 0;
        goto exit;
    }

    header.byte = c->read_buf[0];
    rc = header.bits.type;
    if (c->connect_data->keep_alive_interval > 0)
        platform_timer_cutdown(&c->last_received, c->connect_data->keep_alive_interval); // record the fact that we have successfully received a packet
exit:
    return rc;
}

int mqtt_send_packet(mqtt_client_t* c, int length, platform_timer_t* timer)
{
    int len = 0;
    int sent = 0;

    while ((sent < length) && (!platform_timer_is_expired(timer))) {
        len = c->network->write(c->network, &c->write_buf[sent], length, platform_timer_timer_remain(timer));
        if (len < 0)  // there was an error writing the data
            break;
        sent += len;
    }

    if (sent == length) {
        platform_timer_cutdown(&c->last_sent, c->connect_data->keep_alive_interval); // record the fact that we have successfully sent the packet
        RETURN_ERROR(SUCCESS_ERROR);
    }

    else
        RETURN_ERROR(FAIL_ERROR);
}

int waitfor(mqtt_client_t* c, int packet_type, platform_timer_t* timer)
{
    int rc = FAIL_ERROR;

    do
    {
        if (platform_timer_is_expired(timer))
            break; // we timed out
        rc = cycle(c, timer);
    }
    while (rc != packet_type && rc >= 0);

    return rc;
}

int mqtt_connect_with_results(mqtt_client_t* c, mqtt_connack_data_t* connack_data)
{
    platform_timer_t connect_timer;
    int len = 0;
    int rc = FAIL_ERROR;
    MQTTPacket_connectData connect_data = MQTTPacket_connectData_initializer;

    printf("c->buf = %p, size = %ld\n", c->write_buf, c->write_buf_size);

    if ((NULL == c) || (NULL == connack_data)) 
        RETURN_ERROR(NULL_VALUE_ERROR);

    if (CLIENT_STATE_CONNECTED == mqtt_get_client_state(c))
        RETURN_ERROR(SUCCESS_ERROR);

    platform_timer_init(&connect_timer);
    platform_timer_cutdown(&connect_timer, c->cmd_timeout);

    rc = c->network->connect(c->network);
    if (SUCCESS_ERROR != rc)
        RETURN_ERROR(rc);
    
    connect_data.keepAliveInterval = c->connect_data->keep_alive_interval;
    connect_data.cleansession = c->connect_data->clean_session;
    connect_data.MQTTVersion = c->connect_data->mqtt_version;
    connect_data.clientID.cstring= c->connect_data->client_id;
    connect_data.username.cstring = c->connect_data->user_name;
    connect_data.password.cstring = c->connect_data->password;

    platform_timer_cutdown(&c->last_received, c->connect_data->keep_alive_interval);

    platform_mutex_lock(&c->write_lock);

    if ((len = MQTTSerialize_connect(c->write_buf, c->write_buf_size, &connect_data)) <= 0)
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
    if (rc == SUCCESS_ERROR) {
        c->client_state = CLIENT_STATE_CONNECTED;
        c->ping_outstanding = 0;
    }
    
    platform_mutex_unlock(&c->write_lock);

    RETURN_ERROR(rc);
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

    c->read_buf_size = init->read_buf_size;
    c->write_buf_size =  init->write_buf_size;

    c->packet_id = 1;
    if ((init->cmd_timeout < MIN_CMD_TIMEOUT) || (init->cmd_timeout > MAX_CMD_TIMEOUT))
        c->cmd_timeout = DEFAULT_CMD_TIMEOUT;
    else
        c->cmd_timeout = init->cmd_timeout;
    
    c->connection_state = 0;
    c->ping_outstanding = 0;
    c->connect_data = &init->connect_params;
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

// int mqtt_connect(mqtt_client_t* c)
int mqtt_connect(mqtt_client_t* c)
{
    mqtt_connack_data_t connack_data;
    return mqtt_connect_with_results(c, &connack_data);
}

