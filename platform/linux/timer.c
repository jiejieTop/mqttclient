/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-10 22:16:41
 * @LastEditTime: 2019-12-16 02:03:52
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include "timer.h"

void platform_timer_init(platform_timer_t* timer)
{
    timer->time = (struct timeval){0, 0};
}

void platform_timer_cutdown(platform_timer_t* timer, unsigned int timeout)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval interval = {timeout / 1000, (timeout % 1000) * 1000};
    timeradd(&now, &interval, &timer->time);
}
