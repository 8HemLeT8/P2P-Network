#include "reactor.h"
#include <stdio.h>
#include <sys/select.h>
#include <stdbool.h>


int run()
{
    fd_set rfds;
    fd_set wfds;
    fd_set efds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);

    while(true){
    int retval = select(MAX_FDS, &rfds, &wfds, &efds, NULL);
        
    }
}