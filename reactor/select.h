#ifndef __SELECT_H__
#define __SELECT_H__

int add_fd_to_monitoring(const unsigned int fd);
int32_t wait_for_input();
extern int32_t alloced_fds_num;
extern int32_t *alloced_fds;

#endif