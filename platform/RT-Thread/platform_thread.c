/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-23 19:26:27
 * @LastEditTime: 2020-09-20 14:30:08
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
    platform_thread_t *thread;

    thread = platform_memory_alloc(sizeof(platform_thread_t));

    if(RT_NULL == thread)
    {
        return RT_NULL;
    }

    /*modify thread creation method is dynamic creation, so thread exit rtos can recylcle the resource!*/
    thread->thread = rt_thread_create((const char *)name,
        entry, param,
        stack_size, priority, tick);
    
    if (thread->thread == RT_NULL)
    {
        return RT_NULL;    
    }
    else
    {
        return thread;    
    }

}

void platform_thread_startup(platform_thread_t* thread)
{
    rt_thread_startup(thread->thread);
}


void platform_thread_stop(platform_thread_t* thread)
{
    rt_thread_suspend(thread->thread);
    
}

void platform_thread_start(platform_thread_t* thread)
{
    rt_thread_resume(thread->thread);
}

void platform_thread_destroy(platform_thread_t* thread)
{
    platform_memory_free(thread);
}


