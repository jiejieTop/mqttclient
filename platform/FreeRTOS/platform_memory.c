/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-14 22:02:07
 * @LastEditTime: 2020-04-27 16:32:58
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "platform_memory.h"
#include "string.h"

#include "FreeRTOS.h"

void *platform_memory_alloc(size_t size)
{
    char *ptr;
    ptr = pvPortMalloc(size);
    memset(ptr, 0, size);
    return (void *)ptr;
}

void *platform_memory_calloc(size_t num, size_t size)
{
    return pvPortMalloc(num * size);
}

void platform_memory_free(void *ptr)
{
    vPortFree(ptr);
}



