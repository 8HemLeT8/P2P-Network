#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include "Parser/cli_parser.h"
#include "Reactor/select.h"
#include "Configuration/configuration.h"
#include "Handler/handler.h"
void clean(int temp)
{
    close(LISTENING_FD);
}
int32_t main(int argc, char *argv[])
{
    if (argc < 2)
    {
        perror("Run this node as ./run {PORT}\n");
        return -1;
    }
    PORT = atoi(argv[1]);
    signal(SIGINT, clean);
    /* init node */
    Node node;
    NODE_init(&node, PORT);
    add_fd_to_monitoring(node.sock);

    printf("current node is: %d\n", current_id);
    printf("Please enter your scommand: \n");
    while (true)
    {
        int32_t ready_fd = wait_for_input();
        if (ready_fd == -1)
        {
            perror("");
            return -1;
        }
        else
        {
            handle(ready_fd, &node);
        }
    }
    printf("Error occured!!\n");
    close(LISTENING_FD);
}