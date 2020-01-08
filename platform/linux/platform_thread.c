/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-23 19:26:27
 * @LastEditTime : 2020-01-08 20:23:31
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
    int res;
    platform_thread_t *thread;
    void *(*thread_entry) (void *);

    thread_entry = (void *(*)(void*))entry;
    thread = platform_memory_alloc(sizeof(platform_thread_t));
    res = pthread_create(&thread->thread, NULL, thread_entry, param);
    if(res != 0) {
        platform_memory_free(thread);
    }

    return thread;
}

void platform_thread_destroy(platform_thread_t* thread)
{
    if (NULL != thread)
        pthread_detach(thread->thread);
}


