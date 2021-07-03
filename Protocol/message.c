#include "message.h"
#include <stdio.h>
#include <stddef.h>

static uint32_t id = 0;

bool create_connect_message(int32_t src_id, int32_t dst_id, message *msg)
{
    msg->src_id = src_id;
    msg->dst_id = dst_id;
    msg->trailing_msg = 0;
    msg->msg_id = id++;
    msg->func_id = FUNC_ID_CONNECT;
    return true;
}

bool create_ack_message(int32_t src_id, int32_t dst_id, message *msg)
{
    msg->src_id = src_id;
    msg->dst_id = dst_id;
    msg->trailing_msg = 0;
    msg->msg_id = id++;
    msg->func_id = FUNC_ID_ACK;
    return true;
}
