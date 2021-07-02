#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

int parse_and_check(Node *node, char *input)
{
    bool success = true;
    char *chunk = strtok(input, COMMA);
    if (strcmp(chunk, "setid"))
    {
        success = check_setid(node, input);
        if (!success)
        {
            return -1;
        }
    }
    else if (strcmp(chunk, "connect"))
    {
        success = check_connect(node, input);
        if (!success)
        {
            return -1;
        }
    }
    else if (strcmp(chunk, "send"))
    {
        success = check_send(node, input);
        if (!success)
        {
            return -1;
        }
    }

    while (chunk != NULL)
    {
        printf("%s\n", chunk);
        chunk = strtok(NULL, COMMA);
    }
}

int main()
{
    char buffer[200];
    Node cur;
    while (true)
    {
        fgets(buffer, 200, stdin);
        parse_and_check(&cur, buffer);
    }
}