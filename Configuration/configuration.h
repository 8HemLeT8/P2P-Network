#pragma once
#include <stdint.h>

#define NUMBER_OF_NODES (1)
#define NODE_MAX_NEIGHBORS (10)
#define NODE_FIRST_PORT (6965)
#define NODES_IP ("192.168.1.112")

extern int32_t LISTENING_FD;
extern int32_t current_id;
// extern int32_t LISTENING_FD;