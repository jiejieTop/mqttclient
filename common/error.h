/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 00:42:16
 * @LastEditTime: 2019-12-15 15:43:17
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _ERROR_H_
#define _ERROR_H_

typedef enum error {
    NULL_VALUE_ERROR = -2,
    FAIL_ERROR = -1,
    SUCCESS_ERROR = 0
} error_t;

#define RETURN_ERROR(x) { return x; }

#endif
