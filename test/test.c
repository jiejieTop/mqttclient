/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 21:53:07
 * @LastEditTime : 2020-01-02 23:48:52
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "mqttclient.h"

mqtt_client_t client;
client_init_params_t init_params;

static void reconnect_handler(void* client, void* reconnect_date)
{
    LOG_E("%s:%d %s()...mqtt is reconnecting, reconnect_date is %s", __FILE__, __LINE__, __FUNCTION__, reconnect_date);
}

static void topic_test1_handler(void* client, message_data_t* msg)
{
    (void) client;
    LOG_I("-----------------------------------------------------------------------------------");
    LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    LOG_I("-----------------------------------------------------------------------------------");
}

void *mqtt_unsubscribe_thread(void *arg)
{
    sleep(5);
    mqtt_unsubscribe(&client, "mqtt_topic");
    pthread_exit(NULL);
}

void *mqtt_publish_thread(void *arg)
{
    error_t err;
    char buf[80] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "message: Hello World! his is a publish test...");
    
    msg.qos = 2;
    msg.payload = (void *) buf;
    msg.payloadlen = strlen(buf);
    while(1) {
        // LOG_W("---------------------------------------------------------");
        err = mqtt_publish(&client, "testtopic", &msg);
        LOG_E("mqtt publish err = %d",err);
        // err = mqtt_publish(&client, "mqtt_topic", &msg);
        // LOG_E("err = %d",err);
        // err = mqtt_publish(&client, "mqtt_topic2/sdasd", &msg);
        // LOG_E("err = %d",err);
        // err = mqtt_publish(&client, "mqtt_topic1/sdasd", &msg);
        // LOG_E("err = %d",err);
        sleep(2);
    }
}

int main(void)
{
    int res;
    error_t err;
    pthread_t thread1;
    pthread_t thread2;

    init_params.read_buf_size = 1024;
    init_params.write_buf_size = 1024;
    init_params.reconnect_date = "this is a test";
    init_params.reconnect_handler = reconnect_handler;
    init_params.connect_params.network_params.addr = "129.204.201.235"; //"192.168.1.101";
    init_params.connect_params.network_params.port = 1883;
    init_params.connect_params.user_name = "jiejie";
    init_params.connect_params.password = "123456";
    init_params.connect_params.client_id = "clientid";
    init_params.connect_params.clean_session = 1;
    LOG_I("-------------------hello client----------------------");

    log_init();

    err = mqtt_init(&client, &init_params);
    LOG_E("err = %d",err);

    err = mqtt_connect(&client);
    LOG_E("err = %d",err);
    
    err = mqtt_subscribe(&client, "testtopic", QOS2, topic_test1_handler);
    err = mqtt_subscribe(&client, "mqtt_topic", QOS0, NULL);
    LOG_E("err = %d",err);
    err = mqtt_subscribe(&client, "mqtt_topic1/#", QOS0, NULL);
    LOG_E("err = %d",err);
    err = mqtt_subscribe(&client, "mqtt_topic2/+/abc", QOS1, NULL);
    LOG_E("err = %d",err);
    err = mqtt_subscribe(&client, "mqtt_topic2/+/dd", QOS1, NULL);
    err = mqtt_subscribe(&client, "mqtt_topic2/123/dd", QOS0, NULL);
    err = mqtt_subscribe(&client, "mqtt_topic3/#", QOS2, NULL);
    LOG_E("err = %d",err);

    // res = pthread_create(&thread1, NULL, mqtt_unsubscribe_thread, NULL);
    // if(res != 0) {
    //     LOG_I("create thread2 fail");
    //     exit(res);
    // }
    
    res = pthread_create(&thread2, NULL, mqtt_publish_thread, NULL);
    if(res != 0) {
        LOG_I("create thread2 fail");
        exit(res);
    }

    while (1) {
        sleep(100);
    }
}