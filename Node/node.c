#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include "../Configuration/configuration.h"
#include "node.h"
#include "../Protocol/message.h"
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
// static bool node_init(Node *node)
// {
//     if (node == NULL)
//     {
//         perror("NULL args");
//         return false;
//     }
//     node->sock = socket(AF_INET, SOCK_STREAM, 0);
//     if (node->sock == -1)
//     {
//         perror("Failed creating a sock");
//         return false;
//     }
//     return true;
// }

bool NODE_connect(Node *src_node, char *dst_ip, uint32_t dst_port)
{
    if (src_node == NULL || dst_ip == NULL)
    {
        perror("NULL args in NODE_connect");
        return false;
    }

    int32_t src_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (src_sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    // if(Neighbor_exists(src_node->neighbors, src_node->neighbors_count, ))
    src_node->neighbors_count++;
    src_node->neighbors = realloc(src_node->neighbors, src_node->neighbors_count * sizeof(Neighbor));
    short *src_sock_fd = &src_node->neighbors[src_node->neighbors_count - 1].connection;

    // src_node->neighbors[src_node->neighbors_count-1].id = NODE_get_by_port
    *src_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*src_sock_fd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }

    struct sockaddr_in dst;
    // inet_pton(AF_INET, dst_ip, &(dst_node.sock.sin_addr));
    dst.sin_addr.s_addr = inet_addr(dst_ip);
    dst.sin_port = htons(dst_port);

    dst.sin_family = AF_INET;
    int8_t ret = connect(*src_sock_fd, (struct sockaddr *)&dst, sizeof(dst));
    if (ret == -1)
    {
        perror("Error in connect\n");
        return false;
    }
    
    printf("%d -> (%s,%d) Connected successfully!\n", src_node->id, dst_ip, dst_port);
}

bool NODE_send(Node *node, int32_t id, uint32_t len, char *message)
{
    if (node == NULL || message == NULL)
    {
        perror("NULL args in NODE_send");
        return false;
    }
    short dst_sock = Neghibor_get_sock_by_id(node->neighbors, id);
    if (dst_sock == -1)
    {
        perror("No such neighbor!");
        return false;
    }
    int32_t sent_len = send(dst_sock, message, len, 0);
}

short Neghibor_get_sock_by_id(Neighbor *nodes, int32_t id)
{
    if (nodes == NULL)
    {
        perror("NULL args in Neghibor_get_sock_by_id\n");
        goto Exit;
    }
    for (size_t i = 0; i < NUMBER_OF_NODES; i++)
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

bool NODE_route(Node *node, int32_t id) {}

/****
int32_t main()
{
    //remove this

    Node node;
    node_init(&node);
    char ip[10] = "127.0.0.1";

    NODE_connect(&node, ip, 1337);
    NODE_send(&node, 3, 12, "Barel ha man");
}

***/