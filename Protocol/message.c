#include "message.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/socket.h>

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
    memcpy(msg->payload, "KAKI", 4);
    return true;
}

bool send_message(int32_t sock, message *msg)
{
    if (msg == NULL)
    {
        perror("NULL ARGS in send_message\n");
        return false;
    }
    printf("sending %s\n", msg->payload);
    int32_t retval = send(sock, msg, 10, 0); // CHANGE THE MAGICs
    printf("%d debug 3\n", retval);
    if (retval == -1)
    {
        perror("send failed!\n");
        return false;
    }
    return true;
}

bool send_connect_message(short sock, uint32_t src_node_id, int32_t src_id)
{
    message msg;
    create_connect_message(src_node_id, 0, &msg);
    send(sock, &msg, sizeof(message), 0);
}
