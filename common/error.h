/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 00:42:16
 * @LastEditTime: 2019-12-15 22:35:48
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _ERROR_H_
#define _ERROR_H_

typedef enum error {
    CONNECT_FAIL_ERROR = -4,
    MEM_NOT_ENOUGH_ERROR = -3,
    NULL_VALUE_ERROR = -2,
    FAIL_ERROR = -1,
    SUCCESS_ERROR = 0
} error_t;

#define RETURN_ERROR(x) { return x; }

#endif
