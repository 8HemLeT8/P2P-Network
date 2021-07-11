#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>

#include "handler.h"
#include "../Parser/cli_parser.h"
#include "../Protocol/message.h"
#include "../Reactor/select.h"
#include "../Configuration/configuration.h"

bool handle(int32_t fd, Node *node)
{
    // printf("handeling fd number %d\n", fd);
    char buffer[sizeof(message)];
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

        send_ack_message(new_sock, node->id, node->id, FUNC_ID_CONNECT);
        printf("sent connect ack to %d\n", new_sock);
        // printf("%d -> (%s,%d) Connected successfully!\n", src_node->id, dst_ip, dst_port);
        break;
    default:
        printf("debug 2\n");
        size_t len = recv(fd, buffer, sizeof(message), 0);
        message_parse(node, buffer, len, fd);
        break;
    }
}