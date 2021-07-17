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

bool create_discover_message(int32_t src_id, int32_t dst_id, int32_t target_id, message *msg)
{
    msg->msg_id = id++;
    msg->src_id = src_id;
    msg->dst_id = dst_id;
    msg->trailing_msg = 0; // TODO ??
    msg->func_id = FUNC_ID_DISCOVER;
    memset(msg->payload, 0, sizeof(msg->payload));
    memcpy(&msg->payload[0], &target_id, sizeof(int32_t));
    return true;
}

bool create_send_message(int32_t src_id, int32_t dst_id, char *payload, int32_t len, message *msg)
{
    msg->msg_id = id++;
    msg->src_id = src_id;
    msg->dst_id = dst_id;
    msg->trailing_msg = 0; // TODO ??
    msg->func_id = FUNC_ID_SEND;
    memset(msg->payload, 0, sizeof(msg->payload));
    memcpy(msg->payload, payload, len);
    return true;
}

bool send_message(short sock, int32_t src_id, int32_t dst_id, size_t len, char *data)
{
    message msg;
    bool ret = create_send_message(src_id, dst_id, data, len, &msg);
    if (!ret)
    {
        perror("Failed creating send message\n");
        return false;
    }
    int32_t sent = send(sock, &msg, sizeof(message), 0);
    if (sent < 0)
    {
        perror("Failed in Send!");
        return false;
    }
    return true;
}

bool add_myself_to_route(int32_t my_id, message *route_msg)
{
    int *pointer = (int *)route_msg->payload; //original message id
    pointer++;                                //number of routing elements
    (*pointer)++;                             //add myself to size
    int32_t num_of_elements = *pointer;
    for (int i = 0; i < num_of_elements; i++)
    {
        pointer++;
    }
    *pointer = my_id; //add my id in the last position
    return true;
}
bool create_route_message(int32_t starting_msg_id, int32_t route_size, ...)
{
}
bool send_connect_message(short sock, uint32_t src_node_id)
{
    message msg;
    create_connect_message(src_node_id, 0, &msg);
    int ret = send(sock, &msg, sizeof(message), 0);
    if (ret < 0)
        return false;
    return true;
}

bool send_ack_message(short sock, int32_t src_node_id, int32_t current_node, int32_t payload)
{
    message msg;
    create_ack_message(src_node_id, current_node, payload, &msg);
    send(sock, &msg, sizeof(message), 0);
}

bool send_nack_message(short sock, int32_t src_node_id, int32_t current_node, int32_t payload)
{
    message msg;
    create_nack_message(src_node_id, current_node, payload, &msg);
    send(sock, &msg, sizeof(message), 0);
}

bool send_discover_message(short sock, int32_t src_id, int32_t dst_id, int32_t target_id)
{
    message msg;
    create_discover_message(src_id, dst_id, target_id, &msg);
    int32_t ret = (sock, &msg, sizeof(message), 0);
    if (ret < 0)
        return false;
    return true;
}

static bool parse_ack(Node *node, message *msg, int32_t from_fd)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_ack\n");
        return false;
    }
    printf("msg->payload[0] = %d\n", msg->payload[0]);
    switch (msg->payload[0])
    {
    case FUNC_ID_CONNECT:
        printf("IGOT ack message for connect\n");
        break;
    }
    if (node->id == msg->dst_id)
    {
        int32_t n = Neighbor_get_index_by_ip_port(node->neighbors, node->neighbors_count, from_fd);
        if (n == -1)
        {
            perror("didnt get the neghibor!!\n");
            return false;
        }
        else
        {
            node->neighbors[n].id = msg->src_id;
            return true;
        }
    }
    else
    {
        printf("dst id is: %d but my id is: %d\n", msg->dst_id, node->id);
        return false;
    }
    return true;
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
    size_t new_neighbor_index = NODE_get_neighbor_index_by_fd(node, fd);
    if (new_neighbor_index < 0)
        return false;

    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(fd, (struct sockaddr *)&addr, &addr_size);
    int32_t port = ntohs(addr.sin_port);

    node->neighbors[new_neighbor_index].port = port;
    node->neighbors[new_neighbor_index].id = msg->src_id;
    node->neighbors[new_neighbor_index].ip_addr = addr.sin_addr.s_addr;

    send_ack_message(node->neighbors[new_neighbor_index].connection, node->id, msg->src_id, msg->msg_id);
    printf("sent connect ack to (node id) %d\n", node->neighbors[new_neighbor_index].id);

    return true;
}

static bool parse_discover(Node *node, message *msg)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_discover");
    }
}

static bool parse_route(Node *node, message *msg)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_ack");
    }
    if (msg->dst_id == node->id)
    {
    }
    else
    {
        //FIND OUT TO WHAT SOCKET DO I FORWARD THE ROUTE MSG

        // send_route_message(Neghibor_get_sock_by_id())
    }
}

static bool parse_send(Node *node, message *msg)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_ack");
    }
    printf("GOT: %s\n", msg->payload);
    return true;
}

bool message_parse(Node *node, char *buffer, size_t len, int32_t from_fd)
{
    if (len == 0)
    {
        bool ret = NODE_disconnect_neighbor(node, from_fd);
    }
    if (node == NULL || buffer == NULL)
    {
        perror("Null args in message_parse\n");
    }
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
    case FUNC_ID_SEND:
        printf("Got an SEND message\n");
        success = parse_send(node, msg);
        if (!success)
        {
            perror("Failed parsing send\n");
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}