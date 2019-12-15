/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-10 22:18:32
 * @LastEditTime: 2019-12-16 02:04:02
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

typedef struct platform_timer {
    struct timeval time;
} platform_timer_t;

void platform_timer_init(platform_timer_t* timer);
void platform_timer_cutdown(platform_timer_t* timer, unsigned int timeout);
