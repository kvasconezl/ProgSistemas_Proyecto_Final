#include "../include/func.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <mntent.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#define SERV_IP "127.0.0.1"
// #define SERV_PORT 8888
#define DAEMON_PORT 9000

int main(void) {
    // pid_t sid;   //pid, sid;
    // int logsdaemon;

    // pid = fork();
    // if (pid < 0) {
    //     return 0;
    // }
    // if (pid > 0) {
    //     return 0;
    // }
    // umask(0);

    // Implementando socket para conectar con servidor
    int daesock, servsock;
    struct sockaddr_in serv_addr;
    struct sockaddr_in daem_addr;
    socklen_t sin_size;

    daesock = socket(AF_INET, SOCK_STREAM, 0);
    if (daesock != -1) {
        printf("Socket demon success\n");
        daem_addr.sin_family = AF_INET;
        daem_addr.sin_port = htons(DAEMON_PORT);
        daem_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(&(daem_addr.sin_zero), '\0', 8);
        int b = bind(daesock, (struct sockaddr *) &daem_addr, sizeof(struct sockaddr));
        if (b == -1) {
            printf("Bind daemon error\n");
            exit(1);
        }
        printf("Bind daemon success\n");
        int l = listen(daesock, 100);
        if (l == -1) {
            printf("Listen daemon error\n");
        } else {
            printf("Listen daemon success\n");
        }

    } else {
        printf("Socket daemon error\n");
        exit(1);
    }
    // Fin implementación de socket

    // logsdaemon = open("./log/usb.log", O_WRONLY | O_CREAT, 0600);

    // if (logsdaemon == -1) {
    //     perror("Error fichero log_daemon");
    //     return -1;
    // }

    // // sid = setsid();

    // // if (sid < 0) {
    // //     perror("New SID failed");
    // // }

    // if ((chdir("/")) < 0) {
    //     perror("Error al cambiar directorio de trabajo");
    //     return -1;
    // }

    // close(STDIN_FILENO);
    // close(STDOUT_FILENO);
    // close(STDERR_FILENO);

    while (1) {
        sin_size = sizeof(struct sockaddr_in);
        servsock = accept(daesock, (struct sockaddr *) &serv_addr, &sin_size);
        if (servsock != -1) {
            printf("Accept daemon-server success\n");
        } else {
            printf("Accept deamon-server error\n");
            exit(1);
        }
        char *buff = malloc(1024 * sizeof(char));
        int r = recv(servsock, buff, 1024, 0);   //Implementar de forma que se envíe/reciba todo desde el servidor al daemon

        if (r == -1) {
            printf("Recv daemon-server error\n");
            exit(1);
        }
        printf("Recv daemon-server success\n");
        printf("MENSAJE: %s\n\n", buff);

        // struct udev *udev;
        // udev = udev_new();
        // enumerar_disp_alm_masivo(udev, logsdaemon);
        // sleep(5);
    }
    return 0;
}
