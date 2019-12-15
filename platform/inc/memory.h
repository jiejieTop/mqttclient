/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-14 22:06:35
 * @LastEditTime: 2019-12-14 22:07:56
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _MEMORY_H_
#define _MEMORY_H_
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

void *memory_malloc(size_t size);
void memory_free(void *ptr);

#endif
