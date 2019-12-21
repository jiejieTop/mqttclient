/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 21:53:07
 * @LastEditTime : 2019-12-20 21:35:39
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include "mqttclient.h"


int main(void)
{
    error_t err;

    mqtt_client_t client;
    client_init_params_t init_params;
    init_params.read_buf_size = 1024;
    init_params.write_buf_size = 1024;
    init_params.connect_params.addr = "129.204.201.235"; //"192.168.1.101";
    init_params.connect_params.port = 1883;
    init_params.connect_params.user_name = "jiejie";
    init_params.connect_params.password = "123456";
    init_params.connect_params.client_id = "clientid";

    printf("-------------------hello client----------------------\n");

    err = mqtt_init(&client, &init_params);
    printf("err = %d\n",err);

    err = mqtt_connect(&client);
    printf("err = %d\n",err);


    err = mqtt_subscribe(&client, "mqtt_topic", 0, NULL);
    
    printf("err = %d\n",err);

    // err = mqtt_disconnect(&client);

    // printf("err = %d\n",err);

    while (1)
    {
        mqtt_yield(&client, 5000);
    }
    

}