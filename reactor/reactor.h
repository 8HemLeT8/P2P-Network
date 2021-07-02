#pragma once

#define MAX_FDS 5

int _register(int fd);
int unregister(int fd);
int run();