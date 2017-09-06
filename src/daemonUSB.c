#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libudev.h>
#include <stdio.h>
#include <mntent.h>
#include "Monitor.h"

int main(void) {
	pid_t pid, sid;
	pid = fork();
	if (pid < 0) {
		return -1;
	}
	if (pid > 0) {
		return -1;
	}
	umask(0);
	sid = setsid();
	if (sid < 0) {
		perror("SID fallado");
	}
	if ((chdir("/")) < 0) {
		perror("Error");
		return -1;
	}
	while (1) {
		connectionHandler();
	}

}

