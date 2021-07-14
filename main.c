#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include "Parser/cli_parser.h"
#include "Reactor/select.h"
#include "Configuration/configuration.h"
#include "Reactor/handler.h"
void clean(int temp)
{
    close(LISTENING_FD);
    close(6965);
    close(6966);
}
int32_t main()
{
    signal(SIGINT, clean);
    Node nodes[NUMBER_OF_NODES];
    /* init nodes */
    for (int32_t i = 0; i < NUMBER_OF_NODES; i++)
    {
        NODE_init(&nodes[i], NODE_FIRST_PORT + i);
        add_fd_to_monitoring(nodes[i].sock);
    }
    printf("All nodes are initiated\n");

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
            handle(ready_fd, NODE_get_by_id(nodes, current_id));
        }
    }
    printf("Error occured!!\n");
    close(LISTENING_FD);
    close(6965);
    close(6966);
}