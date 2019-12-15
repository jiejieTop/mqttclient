/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-09 21:31:25
 * @LastEditTime: 2019-12-15 15:52:22
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "mqttclient.h"

int mqtt_init(mqtt_client_t *cli, cli_init_params_t *cli_init)
{
    /* network init */
    cli->network = (network_t*)memory_malloc(sizeof(network_t));
    if (NULL == cli->network) {
        printf("malloc network failed\n");
        RETURN_ERROR(FAIL_ERROR);
    }
    memset(cli->network, 0, sizeof(network_t));

    network_init(cli->network, &cli_init->connect_params);
    


}



