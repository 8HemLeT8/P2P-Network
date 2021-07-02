#include "parser.h"
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

bool check_setid(Node *node, char *string)
{
    strtok(string, COMMA); //ignore the function
    int id = atoi(string);
    if (NULL != strtok(NULL, COMMA))
    {
        return false;
    }
    //setid(id);
    printf("id: %d\n", id);
}
bool check_connect(Node *node, char *string)
{
    strtok(string, COMMA); //ignore the function
    struct sockaddr_in sa;
    char *ip = {0};
    int ret = 0;
    uint32_t port = 0;
    ip = strtok(string, COLON);
    if (ip == NULL)
    {
        return false;
    }
    port = atoi(strtok(NULL, COLON));
    printf("ip: %s ... port: %d \n", ip, port);
    bool success = NODE_connect(node, ip, port);
    return success;
}
bool check_send(Node *node, char *string)
{
    strtok(string, COMMA); //ignore the function
    int id = atoi(strtok(NULL, COMMA));
    uint32_t len = atoi(strtok(NULL, COMMA));
    char *message = strtok(NULL, COMMA);
    if (NULL != strtok(NULL, COMMA))
    {
        return false;
    }
    NODE_send(node, id, len, message);
}
bool check_route(Node *node, char *string)
{
    int id = atoi(string);
    if (NULL != strtok(NULL, COMMA))
    {
        return false;
    }
    Node_route(node, id);
}
// bool check_peers(char *string);

// int main()
// {
//     printf("temp main\n");
//     char test[15] = "127.0.0.1:1337";
//     check_connect(test);
//     char test2[10] = "5";
//     check_setid(test2);
// }