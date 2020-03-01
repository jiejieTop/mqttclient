/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-26 19:11:37
 * @LastEditTime: 2020-02-25 04:01:18
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "salof_defconfig.h"

#ifdef USE_LOG

void *salof_alloc(unsigned int size)
{
    return malloc((size_t)size);
}

void salof_free(void *mem)
{
    free(mem);
}

salof_tcb salof_task_create(const char *name,
                            void (*task_entry)(void *param),
                            void * const param,
                            unsigned int stack_size,
                            unsigned int priority,
                            unsigned int tick)
{
    int res;
    salof_tcb task;
    void *(*__start_routine) (void *);

    __start_routine = (void *(*)(void*))task_entry;
    task = salof_alloc(sizeof(pthread_t));
    res = pthread_create(task, NULL, __start_routine, param);
    if(res != 0) {
        salof_free(task);
    }

    return task;
}

salof_mutex salof_mutex_create(void)
{
    salof_mutex mutex;
    mutex = salof_alloc(sizeof(pthread_mutex_t));
    
    if (NULL != mutex)
	    pthread_mutex_init(mutex, NULL);
    
    return mutex;
}

void salof_mutex_delete(salof_mutex mutex)
{
    pthread_mutex_destroy(mutex);
}

int salof_mutex_pend(salof_mutex mutex, unsigned int timeout)
{
    if (timeout == 0)
        return pthread_mutex_trylock(mutex);
    
    return pthread_mutex_lock(mutex);
}

int salof_mutex_post(salof_mutex mutex)
{
    return pthread_mutex_unlock(mutex);
}

salof_sem salof_sem_create(void)
{
    salof_sem sem;
    sem = salof_alloc(sizeof(sem_t));
    
    if (NULL != sem)
	    sem_init(sem, 0, 0);
    
    return sem;
}

void salof_sem_delete(salof_sem sem)
{
    sem_destroy(sem);
}

int salof_sem_pend(salof_sem sem, unsigned int timeout)
{
    (void) timeout;
    return sem_wait(sem);
}

int salof_sem_post(salof_sem sem)
{
    return sem_post(sem);
}

unsigned int salof_get_tick(void)
{
    return (unsigned int)time(NULL);
}

char *salof_get_task_name(void)
{
    return NULL;
}


int send_buff(char *buf, int len)
{
    fputs(buf, stdout);
	fflush(stdout);
    return len;
}

#endif
