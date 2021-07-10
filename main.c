#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "Parser/parser.h"
#include "Reactor/select.h"
#include "Configuration/configuration.h"
#include "Reactor/handler.h"

int32_t main()
{
    // char buffer[200];
    Node nodes[NUMBER_OF_NODES];
    /* init nodes */
    for (int32_t i = 0; i < NUMBER_OF_NODES; i++)
    {
        NODE_init(&nodes[i], NODE_FIRST_PORT + i);
        add_fd_to_monitoring(nodes[i].sock);
    }
    printf("All nodes are initiated\n");

    printf("current node is: %d\n", current_id);
    printf("Please enter your command: \n");
    while (true)
    {
        int32_t ready_fd = wait_for_input();
        printf("got %d fd as ready to read\n", ready_fd);
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