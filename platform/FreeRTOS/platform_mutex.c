/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:27:19
 * @LastEditTime: 2020-04-27 22:22:27
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_mutex.h"

int platform_mutex_init(platform_mutex_t* m)
{
    m->mutex = xSemaphoreCreateMutex();
    return 0;
}

int platform_mutex_lock(platform_mutex_t* m)
{
    return xSemaphoreTake(m->mutex, portMAX_DELAY);
}

int platform_mutex_trylock(platform_mutex_t* m)
{
    return xSemaphoreTake(m->mutex, 0);
}

int platform_mutex_unlock(platform_mutex_t* m)
{
    return xSemaphoreGive(m->mutex);
}

int platform_mutex_destroy(platform_mutex_t* m)
{
    vSemaphoreDelete(m->mutex);
    return 0;
}
