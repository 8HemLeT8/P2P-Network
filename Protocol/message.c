#include "message.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/socket.h>

static uint32_t id = 0;

bool message_check_format(message *msg)
{
    if (msg->dst_id < 0)
        return false;
    if (msg->src_id < 0)
        return false;
    if (msg->func_id < 0)
        return false;
    if (msg->trailing_msg < 0)
        return false;
    // if (msg->payload < 0)
    //     return false;

    return true;
}

static void debug_print_message(message *msg)
{
    printf("~~~~~ DEBUG ~~~~~\n");
    printf("msg id: %d\n", msg->msg_id);
    printf("src id: %d\n", msg->src_id);
    printf("dst id: %d\n", msg->dst_id);
    printf("func id: %d\n", msg->func_id);
    printf("trailing msg: %d\n", msg->trailing_msg);
    if (msg->func_id == FUNC_ID_ACK || msg->func_id == FUNC_ID_NACK)
    {
        printf("payload: %d\n", (int)msg->payload[0]);
    }
    else
    {
        printf("payload: %s\n", msg->payload);
    }
    printf("~~~~~~~~~~~~~~~~~\n");
}

bool create_connect_message(int32_t src_id, int32_t dst_id, message *msg)
{
    msg->src_id = src_id;
    msg->dst_id = dst_id;
    msg->trailing_msg = 0;
    msg->msg_id = id++;
    msg->func_id = FUNC_ID_CONNECT;
    return true;
}

bool create_ack_message(int32_t src_id, int32_t dst_id, int32_t payload, message *msg)
{
    msg->src_id = src_id;
    msg->dst_id = dst_id;
    msg->trailing_msg = 0;
    msg->msg_id = id++;
    msg->func_id = FUNC_ID_ACK;
    memcpy(msg->payload, &payload, 1);
    return true;
}

bool create_nack_message(int32_t src_id, int32_t dst_id, int32_t payload, message *msg)
{
    msg->src_id = src_id;
    msg->dst_id = dst_id;
    msg->trailing_msg = 0;
    msg->msg_id = id++;
    msg->func_id = FUNC_ID_NACK;
    memset(msg->payload, 0, sizeof(msg->payload));

    memcpy(&msg->payload[0], &payload, sizeof(int32_t));
    return true;
}

bool send_connect_message(short sock, uint32_t src_node_id)
{
    message msg;
    create_connect_message(src_node_id, 0, &msg);
    send(sock, &msg, sizeof(message), 0);
}

bool send_ack_message(short sock, int32_t src_node_id, int32_t current_node, int32_t func_id)
{
    message msg;
    create_ack_message(src_node_id, current_node, func_id, &msg);
    send(sock, &msg, sizeof(message), 0);
}

bool send_nack_message(short sock, int32_t src_node_id, int32_t current_node, int32_t payload)
{
    message msg;
    create_nack_message(src_node_id, current_node, payload, &msg);
    send(sock, &msg, sizeof(message), 0);
}

static bool parse_ack(Node *node, message *msg, int32_t from_fd)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_ack");
        return false;
    }

    if (node->id == msg->dst_id)
    {
        int32_t n = Neighbor_get_index_by_ip_port(node->neighbors, node->neighbors_count, from_fd);
        if (n == -1)
        {
            return false;
        }
        else
        {
            node->neighbors[n].id = msg->src_id;
            return true;
        }
    }
}

static bool parse_nack(Node *node, message *msg, int32_t fd)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_ack");
    }
}

static bool parse_connect(Node *node, message *msg, int32_t fd)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_ack");
        return false;
    }
    if (!NODE_add_neighbor(node, msg->src_id, fd))
        return false;

    return true;
}

static bool parse_discover(Node *node, message *msg)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_ack");
    }
}

static bool parse_route(Node *node, message *msg)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_ack");
    }
}
bool message_parse(Node *node, char *buffer, size_t len, int32_t from_fd)
{
    printf("debug 5\n");
    message *msg = (message *)buffer;
    // debug_print_message(msg);
    switch (msg->func_id)
    {
    case FUNC_ID_ACK:
        printf("%d Got an ack message\n", node->id);
        bool success = parse_ack(node, msg, from_fd);
        if (!success)
        {
            perror("Failed parsing ack");
            return false;
        }
        break;
    case FUNC_ID_NACK:
        printf("Got an nack message\n");
        success = parse_nack(node, msg, from_fd);
        if (!success)
        {
            perror("Failed parsing nack");
            return false;
        }
        break;
    case FUNC_ID_CONNECT:
        printf("%d Got a connect message\n", node->id);
        success = parse_connect(node, msg, from_fd);
        if (!success)
        {
            perror("Failed parsing connect");
            return false;
        }
        break;
    case FUNC_ID_DISCOVER:
        printf("Got an discover message\n");
        success = parse_discover(node, msg);
        if (!success)
        {
            perror("Failed parsing discover");
            return false;
        }
        break;
    case FUNC_ID_ROUTE:
        printf("Got an route message\n");
        success = parse_route(node, msg);
        if (!success)
        {
            perror("Failed parsing route");
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}