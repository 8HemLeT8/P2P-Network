#include "message.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <malloc.h>
#include <stdlib.h>
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
    if (msg->func_id == FUNC_ID_DISCOVER)
    {
        printf("payload: %d\n", atoi(msg->payload));
    }
    else
    {
        printf("payload: %s\n", msg->payload);
    }
    printf("~~~~~~~~~~~~~~~~~\n");
}

int32_t create_connect_message(int32_t src_id, int32_t dst_id, message *msg)
{
    msg->src_id = src_id;
    msg->dst_id = dst_id;
    msg->trailing_msg = 0;
    msg->msg_id = id++;
    msg->func_id = FUNC_ID_CONNECT;
    return msg->msg_id;
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
    memcpy(msg->payload, &target_id, sizeof(int32_t));
    printf("in create, payload has %d\n", (int32_t)(*msg->payload));
    return true;
}

bool create_route_message(int32_t src_id, int32_t dst_id, Route *route, message *msg)
{
    msg->msg_id = id++;
    msg->src_id = src_id;
    msg->dst_id = dst_id;
    msg->trailing_msg = 0;
    msg->func_id = FUNC_ID_ROUTE;
    memset(msg->payload, 0, sizeof(msg->payload));
    memcpy(msg->payload, route, sizeof(Route));
    memcpy(((Route *)msg->payload)->nodes_ids, route->nodes_ids, route->route_len);
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
int32_t send_connect_message(short sock, uint32_t src_node_id)
{
    message msg;
    int32_t msg_id = create_connect_message(src_node_id, 0, &msg);
    int ret = send(sock, &msg, sizeof(message), 0);
    if (ret < 0)
        return -1;
    return msg_id;
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
    printf("sent discovery to %d with target for %d\n", dst_id, target_id);

    message msg;
    create_discover_message(src_id, dst_id, htonl(target_id), &msg);
    int32_t ret = send(sock, &msg, sizeof(message), 0);
    if (ret < 0)
        return false;
    return true;
}

bool send_route_message(short sock, int32_t src_node_id, int32_t dst_node_id, Route *route)
{
    message msg;
    create_route_message(src_node_id, dst_node_id, route, &msg);
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
    printf("ACK\n");
    for (int i = 0; i < node->connect_sent.amount; i++)
    {
        if (node->connect_sent.ids[i] == (msg->payload)[0])
        {
            size_t new_neighbor = NODE_get_neighbor_index_by_fd(node, from_fd);
            node->neighbors[new_neighbor].id = msg->src_id;
            printf("%d\n", msg->src_id);
            node->connect_sent.amount--;
            if (node->connect_sent.amount == 0)
            {

                free(node->connect_sent.ids);
                node->connect_sent.ids = NULL;
            }

            return true;
        }
    }
    /** ADD HERE MORE FUNCTION ACKS HANDLERS **/
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
    RoutingInfo *ri;
    if ((ri = NODE_get_route_info(node, ((int32_t *)msg->payload)[0])) != NULL)
    {
        ri->responds_got++;
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

static bool parse_discover(Node *node, message *msg, short from_fd)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_discover\n");
        return false;
    }
    debug_print_message(msg);
    int32_t target_id = ntohl(msg->payload[0]);
    printf("in parsing discover, the target is %d %d\n", (msg->payload)[0], (msg->payload)[1]);
    if (node->id == target_id)
    {
        Route route;
        route.og_id = msg->msg_id;
        route.route_len = 1;
        route.nodes_ids = (int32_t *)malloc(sizeof(int32_t));
        route.nodes_ids[0] = node->id;
        int32_t dst_id = node->neighbors[NODE_get_neighbor_index_by_fd(node, from_fd)].id;
        bool ret = send_route_message(from_fd, node->id, dst_id, &route);
        if (!ret)
        {
            perror("Failed in send_route_message\n");
            return false;
        }
    }
    /**
    else if(NULL != ()){
                ADD HERE THE SEND BACK A NACK IF THIS IS ALREADY KNOWN TARGET?
    }
    **/
    else
    {
        for (int i = 0; i < node->neighbors_count; i++)
        {
            if (node->neighbors[i].id != msg->src_id)
            {
                bool ret = send_discover_message(node->neighbors[i].connection, node->id, node->neighbors[i].id, target_id);
                if (!ret)
                {
                    perror("Failed in send_discover_message\n");
                    return false;
                }
            }
        }
    }

    return true;
}

static bool parse_route(Node *node, message *msg, short from_fd)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_route");
    }
    Route *route = (Route *)msg->payload;
    bool res = NODE_add_route(node, route);
    if (!res)
    {
        perror("Failed in adding route..\n");
        return false;
    }

    RoutingInfo *ri = NODE_get_route_info(node, route->og_id);
    if (ri == NULL)
    {
        perror("Failed in NODE_get_route_info\n");
        return false;
    }
    // TODO AFTER I FINISH WITH DISCOVER
    if (ri->src_node_id == node->id)
    {
        if (ri->responds_got == node->neighbors_count)
        {
            //got all the route and nacks back
            //choose the best route
            //build the relay message
            Route *best = NODE_choose_route(ri->routes, ri->routes_got);
            printf("I need to send my message to: %d\n", *best->nodes_ids);
        }
    }

    else
    {
        bool ret = add_myself_to_route(node->id, msg);
        if (!ret)
        {
            perror("failed in add_myself_to_route\n");
            return false;
        }
        short dst_sock = Neghibor_get_sock_by_id(node->neighbors, node->neighbors_count, ri->src_node_id);
        ret = send_route_message(dst_sock, node->id, ri->src_node_id, (Route *)msg->payload);
        if (!ret)
        {
            perror("failed in send_route_message\n");
            return false;
        }
    }
}

static bool parse_send(Node *node, message *msg)
{
    if (node == NULL || msg == NULL)
    {
        perror("NULL args in parse_send");
    }
    if (msg->dst_id == node->id)
    {
        printf("GOT: %s\n", msg->payload);
        return true;
    }
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
        success = parse_discover(node, msg, from_fd);
        if (!success)
        {
            perror("Failed parsing discover");
            return false;
        }
        break;
    case FUNC_ID_ROUTE:
        printf("Got an route message\n");
        success = parse_route(node, msg, from_fd);
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