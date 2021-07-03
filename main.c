#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "configuration.h"

int main()
{
    char buffer[200];
    Node nodes[NUMBER_OF_NODES];
    /* init nodes */
    for (int i = 0; i < NUMBER_OF_NODES; i++)
    {
        NODE_init(&nodes[i], NODE_FIRST_PORT + i);
    }
    printf("All nodes are initiated\n");

    while (true)
    {
        printf("Please enter your command: \n");
        fgets(buffer, 200, stdin);
        parse_check_run(NODE_get_by_id(nodes, current_id), buffer);
        printf("current node is: %d\n", current_id);
    }
}