#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "Reactor/select.h"
#include "Configuration/configuration.h"

int main()
{
    char buffer[200];
    Node nodes[NUMBER_OF_NODES];
    /* init nodes */
    for (int i = 0; i < NUMBER_OF_NODES; i++)
    {
        NODE_init(&nodes[i], NODE_FIRST_PORT + i);
        add_fd_to_monitoring(nodes[i].sock);
    }
    printf("All nodes are initiated\n");

    while (true)
    {
        printf("current node is: %d\n", current_id);
        printf("Please enter your command: \n");
        int fds = wait_for_input(); //? - to check
        // printf("got %d fd as ready to read\n", fds);
        //  fds = wait_for_input(); //? - to check
        // printf("got %d fd as ready to read\n", fds);

        //  fds = wait_for_input(); //? - to check
        // printf("got %d fd as ready to read\n", fds);
        //  fds = wait_for_input(); //? - to check

        printf("got %d fd as ready to read\n", fds);
        fgets(buffer, 200, stdin);

        parse_check_run(NODE_get_by_id(nodes, current_id), buffer);
    }
}