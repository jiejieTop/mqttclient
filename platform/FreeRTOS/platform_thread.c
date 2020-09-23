/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-23 19:26:27
 * @LastEditTime: 2020-09-23 08:53:43
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_thread.h"
#include "platform_memory.h"

platform_thread_t *platform_thread_init( const char *name,
                                        void (*entry)(void *),
                                        void * const param,
                                        unsigned int stack_size,
                                        unsigned int priority,
                                        unsigned int tick)
{
    BaseType_t err;
    platform_thread_t *thread;
    
    thread = platform_memory_alloc(sizeof(platform_thread_t));

    (void)tick;

    err =  xTaskCreate(entry, name, stack_size, param, priority, &thread->thread);

    if(pdPASS != err) {
        platform_memory_free(thread);
        return NULL;
    }

    return thread;
}

void platform_thread_startup(platform_thread_t* thread)
{
    (void)thread;
}


void platform_thread_stop(platform_thread_t* thread)
{
    vTaskSuspend(thread->thread);
}

void platform_thread_start(platform_thread_t* thread)
{
    vTaskResume(thread->thread);
}

void platform_thread_destroy(platform_thread_t* thread)
{
    if (NULL != thread)
        vTaskDelete(thread->thread);
    
    platform_memory_free(thread);
}


