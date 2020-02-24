/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:27:19
 * @LastEditTime : 2020-01-08 20:23:13
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_mutex.h"

int platform_mutex_init(platform_mutex_t* m)
{
    m->mutex = rt_mutex_create("platform_mutex", RT_IPC_FLAG_PRIO);
    return 0;
}

int platform_mutex_lock(platform_mutex_t* m)
{
    return rt_mutex_take((m->mutex), RT_WAITING_FOREVER);
}

int platform_mutex_trylock(platform_mutex_t* m)
{
    return rt_mutex_take((m->mutex), 0);
}

int platform_mutex_unlock(platform_mutex_t* m)
{
    return rt_mutex_release((m->mutex));
}

int platform_mutex_destroy(platform_mutex_t* m)
{
    return rt_mutex_delete((m->mutex));
}
