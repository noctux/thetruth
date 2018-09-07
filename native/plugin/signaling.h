#ifndef __GUARD_SIGNALING_H_
#define __GUARD_SIGNALING_H_

int create_hook_fd_signaling(void);
void reset_fd_signaling(void);
void trigger_fd_signaling(void);
int destroy_hook_fd_signaling(void);

#endif
