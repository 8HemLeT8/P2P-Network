#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "../Configuration/configuration.h"
#include "../Protocol/message.h"
#include "../Reactor/select.h"
#include "node.h"

bool NODE_init(Node *node, uint32_t port)
{
    if (node == NULL)
    {
        perror("NULL args in NODE_init");
        return false;
    }
    node->neighbors_count = 0;
    node->neighbors = NULL;
    node->routing_count = 0;
    node->my_routing = NULL;
    node->connect_sent.amount = 0;
    node->connect_sent.ids = NULL;
    node->id = port;

    int32_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("socket creation failed...\n");
        return -1;
    }

    node->sock = sock;
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(NODES_IP);
    servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and port.
    if ((bind(node->sock, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed. Node ID: %d\n", node->id);
        exit(0);
    }
    // Now server is ready to listen
    if ((listen(node->sock, NODE_MAX_NEIGHBORS)) != 0)
    {
        printf("Listen() failed. Node ID: %d\n", node->id);
        exit(0);
    }
    LISTENING_FD = node->sock;
    current_id = port;
    printf("port: %d is listening..\n", port);
}

bool NODE_setid(Node *node, int32_t id)
{
    if (node == NULL)
    {
        perror("NULL args in NODE_setid");
        return false;
    }
    current_id = id;
    node->id = id;
    return true;
}
bool Neighbor_exists(Neighbor *nodes, int32_t size, int32_t id)
{
    if (nodes == NULL)
    {
        perror("NULL args in Neighbor_exists\n");
        goto Exit;
    }
    for (size_t i = 0; i < size; i++)
    {
        if (nodes[i].id == id)
            return true;
    }
Exit:
    return false;
}

bool NODE_add_neighbor(Node *node, int32_t id, int32_t fd)
{
    node->neighbors_count++;
    node->neighbors = realloc(node->neighbors, node->neighbors_count * sizeof(Neighbor));
    node->neighbors[node->neighbors_count - 1].connection = fd;
    /*check if I can add here ip port set as well*/
    return true;
}

bool NODE_connect(Node *src_node, char *dst_ip, uint32_t dst_port)
{
    if (src_node == NULL || dst_ip == NULL)
    {
        perror("NULL args in NODE_connect");
        return false;
    }
    src_node->neighbors_count++;
    src_node->neighbors = realloc(src_node->neighbors, src_node->neighbors_count * sizeof(Neighbor));

    short *src_sock_fd = &src_node->neighbors[src_node->neighbors_count - 1].connection;
    src_node->neighbors[src_node->neighbors_count - 1].ip_addr = inet_addr(dst_ip);
    src_node->neighbors[src_node->neighbors_count - 1].port = dst_port;
    // src_node->neighbors[src_node->neighbors_count-1].id = NODE_get_by_port
    *src_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*src_sock_fd == -1)
    {
        printf("socket creation failed...\n");
        return false;
    }
    // printf("added socket %d as neighbor\n", *src_sock_fd);

    struct sockaddr_in dst;
    dst.sin_addr.s_addr = inet_addr(dst_ip);
    dst.sin_port = htons(dst_port);
    dst.sin_family = AF_INET;
    int8_t ret = connect(*src_sock_fd, (struct sockaddr *)&dst, sizeof(dst));
    if (ret == -1)
    {
        perror("Error in connect\n");
        NODE_disconnect_neighbor(src_node, *src_sock_fd);
        return false;
    }
    add_fd_to_monitoring(*src_sock_fd);
    int32_t msg_id = send_connect_message(*src_sock_fd, src_node->id);
    if (msg_id < 0)
    {
        perror("Failed at send_connect_message\n");
        return false;
    }
    src_node->connect_sent.amount++;
    src_node->connect_sent.ids = realloc(src_node->connect_sent.ids,
                                         src_node->connect_sent.amount * sizeof(int32_t));
    src_node->connect_sent.ids[src_node->connect_sent.amount - 1] = msg_id;
    return true;
}

bool NODE_send(Node *node, int32_t id, uint32_t len, char *data)
{
    if (node == NULL || data == NULL)
    {
        perror("NULL args in NODE_send");
        return false;
    }
    short dst_sock = Neghibor_get_sock_by_id(node->neighbors, node->neighbors_count, id);
    if (dst_sock == -1) // routing...
    {
        if (node->neighbors_count > 0)
        {
            printf("No such neighbor, discovering...\n");

            message *msg = malloc(sizeof(message));
            memcpy(msg, data, len);
            bool res = message_check_format(msg);
            if (!res)
            {
                perror("BAD MESSAGE FORMAT");
                return false;
            }
            msg = (message *)data;
            if (msg->dst_id == node->id)
            {
                printf("SENT MSG TO MYSELF..?..\n");
                /* add logic? */
            }
            for (int i = 0; i < node->neighbors_count; i++)
            {
                bool ret = send_discover_message(node->neighbors[i].connection, node->id,
                                                 node->neighbors[i].id, id);
                if (!ret)
                {
                    perror("Failed in send_discover_message");
                }
            }
        }
        else
        {
            perror("No neghibors found!\n");
            return false;
        }
    }
    else
    {
        bool ret = send_message(dst_sock, node->id, id, len, data);
        if (!ret)
        {
            perror("Failed in send_message");
            return false;
        }
    }
    return true;
}

short Neghibor_get_sock_by_id(Neighbor *nodes, size_t size, int32_t id)
{
    if (nodes == NULL)
    {
        goto Exit;
    }
    for (size_t i = 0; i < size; i++)
    {
        if (nodes[i].id == id)
            return nodes[i].connection;
    }
Exit:
    return -1;
}

Node *NODE_get_by_id(Node *nodes, int32_t id)
{
    if (nodes == NULL)
    {
        perror("NULL args in NODE_get_by_id\n");
        goto Exit;
    }
    for (size_t i = 0; i < NUMBER_OF_NODES; i++)
    {
        if (nodes[i].id == id)
            return &nodes[i];
    }
Exit:
    return NULL;
}

bool NODE_route(Node *node, int32_t id)
{
    if (node == NULL)
    {
        perror("NULL args in NODE_route\n");
        goto Exit;
    }
    if (node->neighbors_count < 1)
    {
        perror("Cant route... no neighbors\n");
        goto Exit;
    }
    for (int i = 0; i < node->neighbors_count; i++)
    {
        bool ret = send_discover_message(node->neighbors[i].connection, node->id, node->neighbors[i].id, id);
        if (!ret)
        {
            perror("Failed in sending discover msg\n");
            goto Exit;
        }
    }
    return true;
Exit:
    return false;
}

int32_t Neighbor_get_index_by_ip_port(Neighbor *neghibors, size_t len, int32_t fd)
{
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(fd, (struct sockaddr *)&addr, &addr_size);
    int32_t port = ntohs(addr.sin_port);

    for (int i = 0; i < len; i++)
    {
        if (neghibors[i].ip_addr == addr.sin_addr.s_addr && neghibors[i].port == ntohs(addr.sin_port))
        {
            return i;
        }
    }
    return -1;
}

size_t NODE_get_neighbor_index_by_fd(Node *node, short fd)
{
    for (int i = 0; i < node->neighbors_count; i++)
    {
        // printf("neighbor fd is %d, but i need %d\n", node->neighbors[i].connection, fd);

        if (node->neighbors[i].connection == fd)
        {
            return i;
        }
    }
    return -1;
}

bool NODE_disconnect_neighbor(Node *node, short fd)
{
    /*
ADD HERE REMOVE FROM REACTOR!!!
*/
    if (node == NULL)
    {
        perror("Null args in NODE_disconnect_neighbor");
        return false;
    }
    int to_rm = NODE_get_neighbor_index_by_fd(node, fd);
    if (to_rm < 0)
    {
        perror("Failed to find neighbor\n");
        return false;
    }
    if ((--node->neighbors_count) <= 0)
    {
        free(node->neighbors);
    }
    else
    {
        Neighbor *temp_arr = (Neighbor *)malloc(node->neighbors_count * sizeof(Neighbor));
        for (int j = 0; j < to_rm; j++)
        {
            memcpy(&temp_arr[j], &node->neighbors[j], sizeof(Neighbor));
        }
        for (int k = to_rm + 1; k < node->neighbors_count; k++)
        {
            memcpy(&temp_arr[k], &node->neighbors[k], sizeof(Neighbor));
        }
        printf("closing connection...\n");
        close(node->neighbors[to_rm].connection);
        free(node->neighbors);
        node->neighbors = temp_arr;
    }
    printf("Removed fd %d\n", fd);
    return true;
}
/**
 * check if it is a route message
 * find the route og_id and add to it the route
 * 
 */

bool NODE_add_route(Node *node, Route *new_route)
{
    if (node == NULL || new_route == NULL)
    {
        perror("NULL args in NODE_add_route\n");
    }
    bool added = false;

    if (node->routing_count == 0)
    {
        node->routing_count++;
        node->my_routing = malloc(sizeof(RoutingInfo));
        node->my_routing[0].og_id = new_route->og_id;
        node->my_routing[0].routes_got++;
        node->my_routing[0].responds_got++;
        node->my_routing[0].routes = malloc(sizeof(Route));
        node->my_routing[0].routes[0].nodes_ids = malloc(sizeof(int32_t) * new_route->route_len);
        memcpy(&node->my_routing[0].routes[0], new_route, (sizeof(new_route))); //need to check
    }

    for (int i = 0; i < node->routing_count; i++) // serach for exsiting og_id
    {
        if (node->my_routing[i].og_id == new_route->og_id)
        {
            node->my_routing[i].routes_got++;
            node->my_routing[i].responds_got++;
            node->my_routing = realloc(node->my_routing[i].routes, node->my_routing[i].routes_got);
            node->my_routing[i].routes[node->my_routing[i].routes_got - 1].og_id = new_route->og_id;
            node->my_routing[i].routes[node->my_routing[i].routes_got - 1].route_len = new_route->route_len;
            memcpy(&node->my_routing[i].routes[node->my_routing[i].routes_got - 1].nodes_ids,
                   new_route->nodes_ids, (sizeof(int32_t) * new_route->route_len));
            added = true;
            break;
        }
    }
    if (!added) /* new routing */
    {
        node->routing_count++;
        node->my_routing = realloc(node->my_routing, node->routing_count * sizeof(RoutingInfo));
        node->my_routing[node->routing_count - 1].og_id = new_route->og_id;
        node->my_routing[node->routing_count - 1].responds_got = 1;
        node->my_routing[node->routing_count - 1].routes_got = 1;
        node->my_routing[node->routing_count - 1].routes = malloc(sizeof(Route) * new_route->route_len);
        node->my_routing[node->routing_count - 1].routes->og_id = new_route->og_id;
        node->my_routing[node->routing_count - 1].routes->route_len = new_route->route_len;
        memcpy(node->my_routing[node->routing_count - 1].routes->nodes_ids, new_route->nodes_ids, sizeof(int32_t) * new_route->route_len);
        added = true;
    }
    return true;
}

RoutingInfo *NODE_get_route_info(Node *node, int32_t route_id)
{
    if (node == NULL)
    {
        perror("NULL args in NODE_get_route_info");
        return NULL;
    }
    for (int i = 0; i < node->routing_count; i++)
    {
        if (node->my_routing[i].og_id = route_id)
        {
            return &node->my_routing[i];
        }
    }
    return NULL;
}
