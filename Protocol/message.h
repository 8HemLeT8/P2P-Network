#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum func_id
{
  FUNC_ID_ACK = 1,
  FUNC_ID_NACK,
  FUNC_ID_CONNECT = 4,
  FUNC_ID_DISCOVER = 8,
  FUNC_ID_ROUTE = 16
} MESSAGE_function_id;

typedef struct message
{
  uint32_t msg_id;
  uint32_t src_id;
  uint32_t dst_id;
  int32_t trailing_msg;
  uint32_t func_id;
  char payload[492];
} message;

bool create_ack_message(int32_t src_id, int32_t dst_id, message* msg);
bool create_nack_message(int32_t src_id, int32_t dst_id);
bool create_connect_message(int32_t src_id, int32_t dst_id, message *msg);
bool create_discover_message(int32_t src_id, int32_t dst_id);
bool create_route_message(int32_t src_id, int32_t dst_id);
