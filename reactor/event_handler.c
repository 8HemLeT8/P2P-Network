#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
/*
int main(void)
{
    fd_set rfds;
    fd_set rfds;
    int retval;

    // Watch stdin (fd 0) to see when it has input. 

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    retval = select(1, &rfds, NULL, NULL, NULL);
    if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is available now.\n");
    // FD_ISSET(0, &rfds) will be true. 
    else
        printf("No data within five seconds.\n");

    exit(EXIT_SUCCESS);
*/