/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-10 22:18:32
 * @LastEditTime: 2019-12-15 14:30:04
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

typedef struct mqtt_timer {
    struct timeval time;
} mqtt_timer_t;

