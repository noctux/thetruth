#include <sys/socket.h>
#include <weechat-plugin.h>
#include <jni.h>
#include <unistd.h>
#include <fcntl.h>

#include "xmpp.h"
#include "signaling.h"

// As weechat can hook fds to schedule callbacks on the main loop
// we use this mechanism to trigger buffer updates anynchronosly
// out of smacks callbacks

// Fds for signaling
static int fd_in  = 0;
static int fd_out = 0;
static struct t_hook *hook = NULL;


int
create_hook_fd_signaling()
{
	int socket_vector[2];
	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, socket_vector)) {
		return WEECHAT_RC_ERROR;
	}

	// Configure fd for nonblocking access
	for (int i = 0; i < 2; i++) {
		int flags = fcntl(socket_vector[i], F_GETFL, 0);
		fcntl(socket_vector[i], F_SETFL, flags | O_NONBLOCK);
	}
	fd_in  = socket_vector[0];
	fd_out = socket_vector[1];

	// Trigger on read evens on fd_in
	hook = weechat_hook_fd(fd_in, 1, 0, 0, &xmpp_pending_operations_cb, NULL, NULL);

	return WEECHAT_RC_OK;
}


void
reset_fd_signaling()
{
#ifdef DEBUG
	weechat_printf(0, "reset_fd_signaling");
#endif
	// Drain the fd to prohibit future callbacks
	// Socket is nonblocking -> simply use read
	char buf[42];
	while(read(fd_in, buf, 1) > 0);
}

void
trigger_fd_signaling()
{
	// writing to fd_out triggers poll with POLLIN in weechat's hook_fd
#ifdef DEBUG
	weechat_printf(0, "trigger_fd_signaling");
#endif
	write(fd_out, "+", 1);
}

int
destroy_hook_fd_signaling()
{
	weechat_unhook(hook);
	hook = NULL;
	close(fd_in);
	close(fd_out);

	return WEECHAT_RC_OK;
}
