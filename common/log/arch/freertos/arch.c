/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-26 19:11:40
 * @LastEditTime : 2019-12-28 01:15:29
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "salof_defconfig.h"

#ifdef USE_LOG

void *salof_alloc(unsigned int size)
{
    return pvPortMalloc(size);
}


void salof_free(void *mem)
{
    vPortFree(mem);
}

salof_tcb salof_task_create(const char *name,
                            void (*task_entry)(void *param),
                            void * const param,
                            unsigned int stack_size,
                            unsigned int priority,
                            unsigned int tick)
{
    salof_tcb task;
    (void)tick;

    xTaskCreate(task_entry, name, stack_size, param, priority, &task);
    return task;
}

salof_mutex salof_mutex_create(void)
{
    return xSemaphoreCreateMutex();
}


void salof_mutex_delete(salof_mutex mutex)
{
    vSemaphoreDelete(mutex);
}


int salof_mutex_pend(salof_mutex mutex, unsigned int timeout)
{
    if(xSemaphoreTake(mutex, timeout) != pdPASS)
        return -1;
    return 0;
}

int salof_mutex_post(salof_mutex mutex)
{
    if(xSemaphoreGive(mutex) != pdPASS)
        return -1;
    return 0;
}


salof_sem salof_sem_create(void)
{
    return xSemaphoreCreateBinary();
}

void salof_sem_delete(salof_sem sem)
{
    vSemaphoreDelete(sem);
}

int salof_sem_pend(salof_sem sem, unsigned int timeout)
{
    if(xSemaphoreTake(sem, timeout) != pdPASS)
        return -1;
    return 0;
}

int salof_sem_post(salof_sem sem)
{
    if(xSemaphoreGive(sem) != pdPASS)
        return -1;
    return 0;
}

unsigned int salof_get_tick(void)
{
    return xTaskGetTickCount();
}

char *salof_get_task_name(void)
{
    return pcTaskGetName(xTaskGetCurrentTaskHandle());
}

#endif
