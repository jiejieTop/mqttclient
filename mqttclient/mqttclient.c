/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime : 2019-12-20 23:10:49
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "mqttclient.h"

static int mqtt_decode_packet(mqtt_client_t* c, int* value, int timeout);
static int mqtt_read_packet(mqtt_client_t* c, platform_timer_t* timer);
int mqtt_send_packet(mqtt_client_t* c, int length, platform_timer_t* timer);

static void default_msg_handler(void* client, message_data_t* msg)
{
    (void*) client;
    printf("%s:%d %s()...\ntopic: %.*s\nmessage:%s\n", 
            __FILE__, __LINE__, __FUNCTION__, 
            msg->topic_name->lenstring.len, msg->topic_name->lenstring.data, (char*)msg->message->payload);
}

client_state_t mqtt_get_client_state(mqtt_client_t* c)
{
    return c->client_state;
}

static int mqtt_get_next_packet_id(mqtt_client_t *c) {
    return c->packet_id = (c->packet_id == MAX_PACKET_ID) ? 1 : c->packet_id + 1;
}

static void mqtt_new_message_data(message_data_t* md, MQTTString* topic_name, mqtt_message_t* message)
{
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

        printf("topic name : %s len %d,data :%s \n",topic_name->cstring, topic_name->lenstring.len, topic_name->lenstring.data);
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
    if (c->connect_params->clean_session)
        mqtt_clean_session(c);
}

int mqtt_keep_alive(mqtt_client_t* c)
{
    int rc = SUCCESS_ERROR;

    if (c->connect_params->keep_alive_interval == 0)
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

int print_str_n(const char* str, unsigned int str_len)  
{  
    unsigned int i=0;  
    for (; i < str_len; ++i)  
        putchar(str[i]);
    return i;  
}

int cycle(mqtt_client_t* c, platform_timer_t* timer)
{
    int len = 0,
        rc = SUCCESS_ERROR;
    int packet_type = mqtt_read_packet(c, timer);     /* read the socket, see what work is due */
    // printf("packet type is %d, time is \n",packet_type);
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

            // printf("c->readbuf: %s,c->readsize:%ld\n", c->read_buf, c->read_buf_size);
            // print_str_n(c->read_buf,c->read_buf_size);

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

    if (mqtt_keep_alive(c) != SUCCESS_ERROR) {
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
    int rc = c->network->read(c->network, c->read_buf, 1, platform_timer_remain(timer));
    if (rc != 1)
        goto exit;

    len = 1;
    /* 2. read the remaining length.  This is variable in itself */
    mqtt_decode_packet(c, &rem_len, platform_timer_remain(timer));

    /* put the original remaining length back into the buffer */
    len += MQTTPacket_encode(c->read_buf + 1, rem_len); 

    if (rem_len > (c->read_buf_size - len)) {
        rc = BUFFER_OVERFLOW_ERROR;
        goto exit;
    }

    /* 3. read the rest of the buffer using a callback to supply the rest of the data */
    if (rem_len > 0 && (rc = c->network->read(c->network, c->read_buf + len, rem_len, platform_timer_remain(timer)) != rem_len)) {
        rc = 0;
        goto exit;
    }

    header.byte = c->read_buf[0];
    rc = header.bits.type;
    if (c->connect_params->keep_alive_interval > 0)
        platform_timer_cutdown(&c->last_received, c->connect_params->keep_alive_interval); // record the fact that we have successfully received a packet
exit:
    return rc;
}

int mqtt_send_packet(mqtt_client_t* c, int length, platform_timer_t* timer)
{
    int len = 0;
    int sent = 0;

    while ((sent < length) && (!platform_timer_is_expired(timer))) {
        len = c->network->write(c->network, &c->write_buf[sent], length, platform_timer_remain(timer));
        if (len < 0)  // there was an error writing the data
            break;
        sent += len;
    }

    if (sent == length) {
        platform_timer_cutdown(&c->last_sent, c->connect_params->keep_alive_interval); // record the fact that we have successfully sent the packet
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
    
    connect_data.keepAliveInterval = c->connect_params->keep_alive_interval;
    connect_data.cleansession = c->connect_params->clean_session;
    connect_data.MQTTVersion = c->connect_params->mqtt_version;
    connect_data.clientID.cstring= c->connect_params->client_id;
    connect_data.username.cstring = c->connect_params->user_name;
    connect_data.password.cstring = c->connect_params->password;

    platform_timer_cutdown(&c->last_received, c->connect_params->keep_alive_interval);

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
    int rc;
    if ((NULL == c) || (NULL == init)) 
        RETURN_ERROR(NULL_VALUE_ERROR);

    memset(c, 0, sizeof(mqtt_client_t));

    /* network init */
    c->network = (network_t*) platform_memory_alloc(sizeof(network_t));
    if (NULL == c->network) {
        printf("malloc network failed...\n");
        RETURN_ERROR(MEM_NOT_ENOUGH_ERROR);
    }
    memset(c->network, 0, sizeof(network_t));


    if (0 == init->read_buf_size)
        init->read_buf_size = DEFAULT_BUF_SIZE;
    if (0 == init->write_buf_size)
        init->write_buf_size = DEFAULT_BUF_SIZE;
    
    c->read_buf = (char*) platform_memory_alloc(init->read_buf_size);
    c->write_buf = (char*) platform_memory_alloc(init->write_buf_size);
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

    if (0 == init->connect_params.keep_alive_interval)
        init->connect_params.keep_alive_interval = KEEP_ALIVE_INTERVAL;
    if (0 == init->connect_params.mqtt_version)
        init->connect_params.mqtt_version = MQTT_VERSION;
    
    init->connect_params.client_id_len = strlen(init->connect_params.client_id);
    init->connect_params.user_name_len = strlen(init->connect_params.user_name);
    init->connect_params.password_len = strlen(init->connect_params.password);
    
    c->connect_params = &init->connect_params;
    c->default_message_handler = default_msg_handler;

    c->network->connect_params = c->connect_params;
    if (rc = network_init(c->network) < 0)
        RETURN_ERROR(rc);

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
        platform_memory_free(c->network);
        c->network = NULL;
    }

    if (NULL != c->read_buf) {
        platform_memory_free(c->read_buf);
        c->read_buf = NULL;
    }

    if (NULL != c->read_buf) {
        platform_memory_free(c->read_buf);
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

int mqtt_disconnect(mqtt_client_t* c)
{
    int rc = FAIL_ERROR;
    platform_timer_t timer;     // we might wait for incomplete incoming publishes to complete
    int len = 0;

    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, c->cmd_timeout);

    platform_mutex_lock(&c->write_lock);

	len = MQTTSerialize_disconnect(c->write_buf, c->write_buf_size);
    if (len > 0)
        rc = mqtt_send_packet(c, len, &timer);            // send the disconnect packet
    mqtt_close_session(c);

    platform_mutex_unlock(&c->write_lock);

    return rc;
}

message_handlers_t *mqtt_msg_handler_create(const char* topic_filter, message_handler_t handler)
{
    message_handlers_t *msg_handler = NULL;

    msg_handler = (message_handlers_t *) platform_memory_alloc(sizeof(message_handlers_t));
    if (NULL == msg_handler)
        return NULL;
    
    list_init(&msg_handler->list);

    if (NULL == handler)
        msg_handler->handler = default_msg_handler;

    msg_handler->handler = handler;
    msg_handler->topic_filter = topic_filter;

    return msg_handler;
}

void *mqtt_msg_handler_destory(message_handlers_t *msg_handler)
{
    list_del(&msg_handler->list);
    platform_memory_free(msg_handler);
}


int mqtt_set_msg_handlers(mqtt_client_t* c, const char* topic_filter, message_handler_t handler)
{
    message_handlers_t *msg_handler = NULL;

    msg_handler = mqtt_msg_handler_create(topic_filter, handler);
    if (NULL == msg_handler)
        RETURN_ERROR(MEM_NOT_ENOUGH_ERROR);

    list_add(&msg_handler->list, &c->msg_handler_list);

    RETURN_ERROR(SUCCESS_ERROR);
}

int mqtt_subscribe_with_results(mqtt_client_t* c, const char* topic_filter, mqtt_qos_t qos, message_handler_t handler)
{
    int rc = FAIL_ERROR;
    int len = 0;
    int count = 0;
    int granted_qos = 0;
    unsigned short packid;
    platform_timer_t timer;
    MQTTString topic = MQTTString_initializer;
    topic.cstring = (char *)topic_filter;
    
    if (CLIENT_STATE_CONNECTED != mqtt_get_client_state(c))
        goto exit;
    
    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, c->cmd_timeout);

    platform_mutex_lock(&c->write_lock);

    len = MQTTSerialize_subscribe(c->write_buf, c->write_buf_size, 0, mqtt_get_next_packet_id(c), 1, &topic, (int*)&qos);
    if (len <= 0)
        goto exit;
    if ((rc = mqtt_send_packet(c, len, &timer)) != SUCCESS_ERROR) // send the subscribe packet
        goto exit;             // there was a problem

    if (waitfor(c, SUBACK, &timer) == SUBACK) {
        if (MQTTDeserialize_suback(&packid, 1, &count, (int*)&granted_qos, c->read_buf, c->read_buf_size) == 1) {
            if (SUBFAIL != granted_qos)
                rc = mqtt_set_msg_handlers(c, topic_filter, handler);
            else {
                rc = MQTT_SUBSCRIBE_QOS_ERROR;
                goto exit;
            }
        }
    }
    else
        rc = FAIL_ERROR;

exit:
    if (rc == FAIL_ERROR)
        mqtt_close_session(c);

    platform_mutex_unlock(&c->write_lock);

    RETURN_ERROR(rc);
}

int mqtt_subscribe(mqtt_client_t* c, const char* topic_filter, mqtt_qos_t qos, message_handler_t msg_handler)
{
    return mqtt_subscribe_with_results(c, topic_filter, qos, msg_handler);
}


int mqtt_yield(mqtt_client_t* c, int timeout_ms)
{
    int rc = SUCCESS_ERROR;
    platform_timer_t timer;

    platform_timer_init(&timer);
    platform_timer_cutdown(&timer, c->cmd_timeout);

	do {
        if (cycle(c, &timer) < 0) {
            rc = FAIL_ERROR;
            break;
        }
  	} while (!platform_timer_is_expired(&timer));

    return rc;
}