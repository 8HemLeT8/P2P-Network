#include "node.h"
#include <arpa/inet.h>
#include <stdio.h>

bool NODE_setid(Node *node, int32_t id)
{
    node->id = id;
    return true;
}

static bool node_init(Node *node)
{
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

bool NODE_send(Node *node, int32_t id, uint32_t len, char *message){
    int sent_len = send(node->sock, message, len, 0);
}

int main()
{
    //remove this

    Node node;
    node_init(&node);
    char ip[10] = "127.0.0.1";

    NODE_connect(&node, ip, 1337);
    NODE_send(&node, 3, 12, "Barel ha man");
}