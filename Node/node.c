#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include "../configuration.h"
#include "node.h"
bool NODE_init(Node *node, uint32_t port)
{
    if (node == NULL)
    {
        perror("NULL args");
        return false;
    }

    node->id = port;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("socket creation failed...\n");
        return -1;
    }

    printf("port: %d\n", port);
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
}

bool NODE_setid(Node *node, int32_t id)
{
    if (node == NULL)
    {
        perror("NULL args");
        return false;
    }

    node->id = id;
    current_id = id;
    return true;
}

static bool node_init(Node *node)
{
    if (node == NULL)
    {
        perror("NULL args");
        return false;
    }
    node->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (node->sock == -1)
    {
        perror("Failed creating a sock");
        return false;
    }
    return true;
}

bool NODE_connect(Node *src_node, char *dst_ip, uint32_t dst_port)
{
    if (src_node == NULL || dst_ip == NULL)
    {
        perror("NULL args");
        return false;
    }
    // Node *new = node->neighbors[node->neighbors_count++];
    Node dst_node;
    struct sockaddr_in dst;

    // inet_pton(AF_INET, dst_ip, &(dst_node.sock.sin_addr));
    dst.sin_addr.s_addr = inet_addr(dst_ip);
    dst.sin_port = htons(dst_port);
    dst.sin_family = AF_INET;
    int8_t ret = connect(src_node->sock, (struct sockaddr *)&dst, sizeof(dst));
    if (ret == -1)
    {
        perror("Error in connect");
    }
}

bool NODE_send(Node *node, int32_t id, uint32_t len, char *message)
{
    if (node == NULL || message == NULL)
    {
        perror("NULL args");
        return false;
    }
    int sent_len = send(node->sock, message, len, 0);
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
int main()
{
    //remove this

    Node node;
    node_init(&node);
    char ip[10] = "127.0.0.1";

    NODE_connect(&node, ip, 1337);
    NODE_send(&node, 3, 12, "Barel ha man");
}

***/