#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
typedef struct neighbor
{
    int32_t id;
    short connection;
} Neighbor;

typedef struct node
{
    int id;
    short sock;
    Neighbor *neighbors;
    uint8_t neighbors_count;
} Node;

bool NODE_init(Node *node, uint32_t port);
bool NODE_setid(Node *node, int32_t id);
bool NODE_connect(Node *node, char *ip, uint32_t port);
bool NODE_send(Node *node, int32_t id, uint32_t len, char *message);
bool NODE_route(Node *node, int32_t id);
Node *NODE_get_by_id(Node *nodes, int32_t id);