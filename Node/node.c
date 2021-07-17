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
    if ((listen(node->sock, NUMBER_OF_NODES)) != 0)
    {
        printf("Listen() failed. Node ID: %d\n", node->id);
        exit(0);
    }
    LISTENING_FD = node->sock;
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
    printf("neghibors count ++ Node_add_neighbor\n");
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
    printf("neghibors count ++ NODE_connect\n");
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
    printf("added socket %d as neighbor\n", *src_sock_fd);

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
    bool val = send_connect_message(*src_sock_fd, src_node->id);
    if (!val)
    {
        perror("Failed at send_connect_message\n");
        return false;
    }
    return true;
}

bool NODE_send(Node *node, int32_t id, uint32_t len, char *str)
{
    if (node == NULL || str == NULL)
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
            memcpy(msg, str, len);
            bool res = message_check_format(msg);
            if (!res)
            {
                perror("BAD MESSAGE FORMAT");
                return false;
            }
            free(msg);
            msg = (message *)str;
            if (msg->dst_id == node->id)
            {
                // send_ack_message(Neghibor_get_sock_by_id(   ))
            }
        }
        perror("No neghibors found!\n");
        return false;
    }
    int32_t sent_len = send(dst_sock, str, len, 0);
}

short Neghibor_get_sock_by_id(Neighbor *nodes, size_t size, int32_t id)
{
    if (nodes == NULL)
    {
        perror("NULL args in Neghibor_get_sock_by_id\n");
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
