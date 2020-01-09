/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-09 19:25:05
 * @LastEditTime : 2020-01-10 00:50:29
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdlib.h>
#include "random.h"
#include "platform_timer.h"
#include "platform_memory.h"

static long last_seed = 1;

static long do_random(unsigned long seed)
{
    srand(seed);
    return rand();
}

long random_number(void)
{
    unsigned long seed = platform_timer_now();
    last_seed += (seed >> ((seed ^ last_seed) % 3));
    return do_random(seed & last_seed);
}

char *random_string(int len)
{
    int flag, i;
    unsigned long random;
    char *str = platform_memory_alloc((size_t)(len + 1));
    if (NULL == str)
        return NULL;
    
    unsigned long seed = random_number();
    seed += (unsigned long)((unsigned long)str ^ seed);
    
    random = do_random(seed);
    
	for (i = 0; i < len; i++) {
		flag = random % 3;
		switch (flag) {
            case 0:
                str[i] = 'A' + do_random(random ^ (i & flag)) % 26;
                break;
            case 1:
                str[i] = 'a' + do_random(random ^ (i & flag)) % 26;
                break;
            case 2:
                str[i] = '0' + do_random(random ^ (i & flag)) % 10;
                break;
            default:
                str[i] = 'x';
                break;
		}
        random += (0xb433e5c6 << i);
	}

    str[len] = '\0';
	return str;
}

