/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-10 22:16:41
 * @LastEditTime: 2019-12-15 14:41:40
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include "timer.h"

void timer_init(mqtt_timer_t* timer)
{
    timer->time = (struct timeval){0, 0};
}

void timer_cutdown(mqtt_timer_t* timer, int millisecond)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval interval = {millisecond / 1000, (millisecond % 1000) * 1000};
    timeradd(&now, &interval, &timer->time);
}
