#include "func.h"
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

int main(void) {
    pid_t pid, sid;
    int logsdaemon;

    pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid > 0) {
        return -1;
    }
    umask(0);

    logsdaemon = open("./log/usb.log", O_WRONLY | O_CREAT, 0600);

    if (logsdaemon == -1) {
        perror("Error fichero log_daemon");
        return -1;
    }

    sid = setsid();

    if (sid < 0) {
        perror("New SID failed");
    }

    if ((chdir("/")) < 0) {
        perror("Error al cambiar directorio de trabajo");
        return -1;
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1) {
        struct udev *udev;
        udev = udev_new();
        enumerar_disp_alm_masivo(udev, logsdaemon);
        sleep(5);
    }
    return -1;
}
