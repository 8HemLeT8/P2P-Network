#pragma once
#include "../Node/node.h"

extern int32_t LISTENING_FD;
bool handle(int32_t fd, Node *node);
