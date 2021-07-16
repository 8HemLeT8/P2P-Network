#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
typedef struct neighbor
{
    int32_t id;
    short connection;
    uint32_t ip_addr;
    uint32_t port;
} Neighbor;

typedef struct node
{
    int32_t id;
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
short Neghibor_get_sock_by_id(Neighbor *nodes, size_t size, int32_t id);
int32_t Neighbor_get_index_by_ip_port(Neighbor *neghibors, size_t len, int32_t fd);
bool Neighbor_exists(Neighbor *nodes, int32_t size, int32_t id);
bool NODE_add_neighbor(Node *node, int32_t id, int32_t fd);
bool NODE_disconnect_neighbor(Node* node, short fd);