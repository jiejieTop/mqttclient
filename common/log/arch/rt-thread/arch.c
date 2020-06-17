/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-26 19:11:34
 * @LastEditTime: 2020-06-17 16:25:18
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "salof_defconfig.h"

#ifdef SALOF_USING_LOG

void *salof_alloc(unsigned int size)
{
    return rt_malloc(size);
}


void salof_free(void *mem)
{
    rt_free(mem);
}

salof_tcb salof_task_create(const char *name,
                            void (*task_entry)(void *param),
                            void * const param,
                            unsigned int stack_size,
                            unsigned int priority,
                            unsigned int tick)
{
    salof_tcb task;

    task =  rt_thread_create((const char *)name,
                                         task_entry,
                                         param,
                                         stack_size,
                                         priority,
                                         tick);
    rt_thread_startup(task);
    return task;
}

salof_mutex salof_mutex_create(void)
{
    return rt_mutex_create("salof_mutex", RT_IPC_FLAG_PRIO);
}

void salof_mutex_delete(salof_mutex mutex)
{
    rt_mutex_delete(mutex);
}


int salof_mutex_pend(salof_mutex mutex, unsigned int timeout)
{

    if(rt_mutex_take((salof_mutex)mutex, timeout) != RT_EOK)
        return -1;
    return 0;
}

int salof_mutex_post(salof_mutex mutex)
{
    if(rt_mutex_release((salof_mutex)mutex) != RT_EOK)
        return -1;
    return 0;
}

salof_sem salof_sem_create(void)
{
    return rt_sem_create("salof_sem", 0, RT_IPC_FLAG_PRIO);
}

void salof_sem_delete(salof_sem sem)
{
    rt_sem_delete((salof_sem)sem);
}


int salof_sem_pend(salof_sem sem, unsigned int timeout)
{

    if(rt_sem_take((salof_sem)sem, timeout) != RT_EOK)
        return -1;
    return 0;
}

int salof_sem_post(salof_sem sem)
{
    if(rt_sem_release((salof_sem)sem) != RT_EOK)
        return -1;
    return 0;
}


unsigned int salof_get_tick(void)
{
    return rt_tick_get();
}

char *salof_get_task_name(void)
{
    return NULL;
}

static rt_device_t new_device = RT_NULL;

int send_buff(char *buf, int len)
{
    /* find new console device */
    if (new_device == RT_NULL)
    {
        new_device = rt_device_find(RT_CONSOLE_DEVICE_NAME);
//        rt_device_open(new_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_STREAM);
    }
#ifdef RT_USING_DEVICE
    if (new_device == RT_NULL)
    {
        rt_hw_console_output(buf);
    }
    else
    {
        rt_uint16_t old_flag = new_device->open_flag;

        new_device->open_flag |= RT_DEVICE_FLAG_STREAM;
        rt_device_write(new_device, 0, buf, rt_strlen(buf));
        new_device->open_flag = old_flag;
    }
#else
    rt_hw_console_output(buf);
#endif
    return len;
}

#endif

