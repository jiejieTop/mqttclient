/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-14 22:02:07
 * @LastEditTime: 2019-12-14 23:28:29
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include "memory.h"


void *memory_malloc(size_t size)
{
    return malloc(size);
}

void memory_free(void *ptr)
{
    free(ptr);
}



