#include "parser.h"
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

int32_t parse_check_run(Node *node, char *input)
{
    if (node == NULL || input == NULL)
    {
        perror("NULL ARGS IN parse_check_run");
        goto Exit;
    }
    bool success = true;
    char *chunk = strtok(input, COMMA);
    if (chunk == NULL) // if there is no comma in the input
    {
        goto Exit;
    }
    if (strcmp(chunk, "setid") == 0)
    {
        success = check_setid(node, chunk);
        if (!success)
        {
            return -1;
        }
    }
    else if (strcmp(chunk, "connect") == 0)
    {
        success = check_connect(node, chunk);
        if (!success)
        {
            return -1;
        }
    }
    else if (strcmp(chunk, "send") == 0)
    {
        success = check_send(node, input);
        if (!success)
        {
            return -1;
        }
    }
    else
    {
        goto Exit;
    }

    while (chunk != NULL)
    {
        printf("%s\n", chunk);
        chunk = strtok(NULL, COMMA);
    }
    return 1;
Exit:
    printf("BAD INPUT\n");
    return -1;
}

bool check_setid(Node *node, char *string)
{
    if (node == NULL || string == NULL)
    {
        perror("NULL ARGS IN setid");
        goto Exit;
    }
    bool success = false;
    char *chr = strtok(NULL, COMMA); //ignore the function
    int32_t id = atoi(chr);
    if (NULL != strtok(NULL, COMMA))
    {
        return false;
    }
    return NODE_setid(node, id);
    printf("id: %d\n", id);
Exit:
    return success;
}
bool check_connect(Node *node, char *string)
{
    if (node == NULL || string == NULL)
    {
        perror("NULL ARGS IN check_connect");
        goto Exit;
    }
    struct sockaddr_in sa;
    char *ip = {0};
    int32_t ret = 0;
    uint32_t port = 0;
    bool success = false;

    ip = strtok(NULL, COLON);
    if (ip == NULL)
    {
        return false;
    }
    char *port_str = strtok(NULL, COLON);
    if (port_str == NULL)
        return false;
    port = atoi(port_str);
    printf("ip: %s | port: %d \n", ip, port);
    success = NODE_connect(node, ip, port);
Exit:
    return success;
}

bool check_send(Node *node, char *string)
{
    if (node == NULL || string == NULL)
    {
        perror("NULL ARGS IN check_send");
        goto Exit;
    }
    bool success = false;
    // strtok(NULL, COMMA); //ignore the function
    int32_t id = atoi(strtok(NULL, COMMA));
    uint32_t len = atoi(strtok(NULL, COMMA));
    char *message = strtok(NULL, COMMA);
    if (NULL != strtok(NULL, COMMA))
    {
        return false;
    }
    success = NODE_send(node, id, len, message);
Exit:
    return success;
}
bool check_route(Node *node, char *string)
{
    if (node == NULL || string == NULL)
    {
        perror("NULL ARGS IN check_route");
        goto Exit;
    }
    bool success = false;
    int32_t id = atoi(string);
    if (NULL != strtok(NULL, COMMA))
    {
        return false;
    }
    success = NODE_route(node, id);
Exit:
    return success;
}
// bool check_peers(char *string);

// int32_t main()
// {
//     printf("temp main\n");
//     char test[15] = "127.0.0.1:1337";
//     check_connect(test);
//     char test2[10] = "5";
//     check_setid(test2);
// }