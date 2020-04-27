/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-10 22:16:41
 * @LastEditTime: 2020-04-27 22:35:34
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include "platform_timer.h"
#include "FreeRTOS.h"
#include "task.h"

static uint32_t platform_uptime_ms(void)
{
#if (configTICK_RATE_HZ == 1000)
    return (uint32_t)xTaskGetTickCount();
#else
    TickType_t tick = 0u;

    tick = xTaskGetTickCount() * 1000;
    return (uint32_t)((tick + configTICK_RATE_HZ - 1) / configTICK_RATE_HZ);
#endif
}

void platform_timer_init(platform_timer_t* timer)
{
    timer->time = 0;
}

void platform_timer_cutdown(platform_timer_t* timer, unsigned int timeout)
{
	timer->time = platform_uptime_ms();
    timer->time += timeout;
}

char platform_timer_is_expired(platform_timer_t* timer)
{
	return platform_uptime_ms() > timer->time ? 1 : 0;
}

int platform_timer_remain(platform_timer_t* timer)
{
    uint32_t now;

    now = platform_uptime_ms();
    if (timer->time <= now) {
        return 0;
    }

    return timer->time - now;
}

unsigned long platform_timer_now(void)
{
    return (unsigned long) platform_uptime_ms();
}

void platform_timer_usleep(unsigned long usec)
{

    TickType_t tick;

    if(usec != 0) {
        tick = usec / portTICK_PERIOD_MS;
        
        if (tick == 0)
            tick = 1;
    }

    vTaskDelay(tick);
}

