/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-15 00:42:16
 * @LastEditTime : 2020-01-02 22:25:38
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#ifndef _ERROR_H_
#define _ERROR_H_

typedef enum error {
    MQTT_ACK_HANDLER_NUM_TOO_MUCH = -22,
    MQTT_RESUBSCRIBE_ERROR = -21,
    MQTT_SUBSCRIBE_ERROR = -20,
    MQTT_SEND_PACKET_ERROR = -19,
    MQTT_SERIALIZE_PUBLISH_ACK_PACKET_ERROR = -18,
    MQTT_PUBLISH_PACKET_ERROR = -17,
    MQTT_RECONNECT_TIMEOUT_ERROR = -16,
    MQTT_SUBSCRIBE_NOT_ACK_ERROR = -15,
    MQTT_NOT_CONNECT_ERROR = -14,
    MQTT_SUBSCRIBE_ACK_PACKET_ERROR = -13,
    MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR = -12,
    MQTT_PUBLISH_ACK_PACKET_ERROR = -11,
    MQTT_PUBLISH_ACK_TYPE_ERROR = -10,
    MQTT_REC_PACKET_ERROR = -9,
    MQTT_BUF_TOO_SHORT_ERROR = -8,
    MQTT_NOTHING_TO_READ_ERROR = -7,
    MQTT_SUBSCRIBE_QOS_ERROR = -6,
    BUFFER_OVERFLOW_ERROR = -5,
    CONNECT_FAIL_ERROR = -4,
    MEM_NOT_ENOUGH_ERROR = -3,
    NULL_VALUE_ERROR = -2,
    FAIL_ERROR = -1,
    SUCCESS_ERROR = 0
} error_t;

#define RETURN_ERROR(x) { return x; }

#endif
