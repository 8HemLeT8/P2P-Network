#pragma once
#include <stdint.h>

typedef struct relay_payload
{
    uint32_t next_node_id;
    uint32_t left_to_relay;
    char payload[]; //all the realy sub messages
} Relay;