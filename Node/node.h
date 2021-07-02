#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>

#define MAX_NEIGHBORS 10

typedef struct node
{
    int id;
    short sock;
    // struct node neighbors[MAX_NEIGHBORS];
    struct node* neighbors;
    uint8_t neighbors_count;
} Node;

bool NODE_setid(Node *node, int32_t id);
bool NODE_connect(Node *node, char *ip, uint32_t port);
bool NODE_send(Node *node, int32_t id, uint32_t len, char *message);
bool NODE_route(Node *node, int32_t id);
