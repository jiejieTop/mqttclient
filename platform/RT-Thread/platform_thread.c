/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-23 19:26:27
 * @LastEditTime: 2020-04-25 08:44:24
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
    rt_err_t err;
    platform_thread_t *thread;
    uint8_t *thread_stack;

    thread = platform_memory_alloc(sizeof(platform_thread_t));

    thread_stack = (uint8_t*) platform_memory_alloc(stack_size);

    err =  rt_thread_init(&(thread->thread),
                            (const char *)name,
                                         entry,
                                         param,
                                         thread_stack,
                                         stack_size,
                                         priority,
                                         tick);
    if(err != RT_EOK) {
        platform_memory_free(thread);
        platform_memory_free(thread_stack);
        return NULL;
    }


    return thread;
}

void platform_thread_startup(platform_thread_t* thread)
{
    rt_thread_startup(&(thread->thread));
}


void platform_thread_stop(platform_thread_t* thread)
{
    rt_thread_suspend(&(thread->thread));
    rt_schedule();
}

void platform_thread_start(platform_thread_t* thread)
{
    rt_thread_resume(&(thread->thread));
}

void platform_thread_destroy(platform_thread_t* thread)
{
    if (NULL != thread)
        rt_thread_detach(&(thread->thread));

    platform_memory_free(&(thread->thread));
    platform_memory_free(&(thread->thread.stack_addr));
}


