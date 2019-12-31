/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 18:31:44
 * @LastEditTime : 2019-12-31 12:42:58
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>

typedef struct platform_thread {
    pthread_t thread;
} platform_thread_t;

platform_thread_t *platform_thread_init( const char *name,
                                        void (*entry)(void *),
                                        void * const param,
                                        unsigned int stack_size,
                                        unsigned int priority,
                                        unsigned int tick);


#endif
