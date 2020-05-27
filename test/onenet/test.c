/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-04-18 12:37:34
 * @LastEditTime: 2020-05-27 11:00:07
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "mqttclient.h"

extern const char *test_ca_get();

mqtt_client_t client;
client_init_params_t init_params;

static void interceptor_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

void *mqtt_publish_thread(void *arg)
{
    char buf[100] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "welcome to mqttclient, this is a publish test...");
    
    msg.qos = 0;
    msg.payload = (void *) buf;
    while(1) {
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());
        mqtt_publish(&client, "temp_hum", &msg);
        sleep(4);
    }
}

int main(void)
{
    int res;
    // pthread_t thread1;
    pthread_t thread2;
    
    printf("\nwelcome to mqttclient test...\n");

    mqtt_log_init();

    init_params.read_buf_size = 1024;
    init_params.write_buf_size = 1024;

    init_params.network.port = "6002";    // onenet platform
    init_params.network.host = "183.230.40.39"; 

    init_params.connect_params.user_name = "348547";
    init_params.connect_params.password = "mqttclienttest1"; 
    init_params.connect_params.client_id = "599908192";
    init_params.connect_params.clean_session = 1;

    mqtt_init(&client, &init_params);

    mqtt_connect(&client);
    
    mqtt_subscribe(&client, "temp_hum", QOS0, NULL);

    mqtt_set_interceptor_handler(&client, interceptor_handler);     // set interceptor handler
    
    res = pthread_create(&thread2, NULL, mqtt_publish_thread, NULL);
    if(res != 0) {
        MQTT_LOG_E("create mqtt publish thread fail");
        exit(res);
    }

    while (1) {
        sleep(100);
    }
}