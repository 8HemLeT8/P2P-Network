#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>

#include "handler.h"
#include "../Parser/parser.h"
#include "../Protocol/message.h"
#include "../Reactor/select.h"
#include "../Configuration/configuration.h"

bool handle(int32_t fd, Node *node)
{
    printf("handeling fd number %d\n", fd);
    char buffer[200];
    int32_t new_sock = 0;
    switch (fd)
    {
    case STDIN_FILENO:
        printf("debug 0\n");

        fgets(buffer, 200, stdin);
        parse_check_run(node, buffer);
        break;
    case LISTENING_FD:
        printf("debug 1\n");
        new_sock = accept(fd, NULL, NULL);
        if (new_sock < 0)
        {
            // if (errno != EWOULDBLOCK)    ???
            perror("  accept() failed");
            break;
        }
        add_fd_to_monitoring(new_sock);
        message msg;
        // printf("THIS ARE THE MONITORED FDs:\n");
        // for (int32_t i = 0; i < alloced_fds_num; i++)
        // {
        //     printf("fd %d\n", alloced_fds[i]);
        // }
        // send_ack_message(node->id, 0, &msg);
        printf("sending to %d\n", new_sock);
        send_message(new_sock, &msg);
        break;
    default:
        printf("debug 2\n");
        recv(fd, buffer, 200, 0);
        printf("%d got: %s\n", fd, buffer);
        // parse_check_run(buffer);
        break;
    }
}