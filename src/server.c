#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif

#include <arpa/inet.h>
#include <microhttpd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888
#define PRIV_PORT 9800
#define DAEM_PORT 9000

int sockfd;
struct sockaddr_in addr;
struct sockaddr_in d_addr;

static int connection_handler(void *cls, struct MHD_Connection *connection,
                              const char *url, const char *method,
                              const char *version, const char *upload_data,
                              size_t *upload_data_size, void **con_cls) {
    char *page = "<html><body></body></html>";
    struct MHD_Response *response;
    int ret;

    if (strcmp("GET", method) == 0) {
        // printf("GET ");
        if (strcmp("/listar_dispositivos", url) == 0) {
            // printf("/listar_dispositivos\n");
            char *msg = "hola mundo";
            int s = send(sockfd, msg, sizeof(msg), 0);
            if (s != -1) {
                printf("Send server-daemon success, msglen: %d\n", s);
            } else {
                printf("Error en send serv\n");
            }
        } else if (strcmp("/leer_archivo", url) == 0) {
            printf("/leer_archivo\n");
        }
    } else if (strcmp("POST", method) == 0) {
        printf("POST ");
        if (strcmp("/nombrar_dispositivo", url) == 0) {
            printf("/nombrar_dispositivo\n");
        } else if (strcmp("/escribir_archivo", url) == 0) {
            printf("/escribir_archivo\n");
        }
    }

    response = MHD_create_response_from_buffer(strlen(page), (void *) page,
                                               MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);

    MHD_destroy_response(response);

    return ret;
}

int main(void) {
    struct MHD_Daemon *daemon;

    //Implementación socket con puerto privado para conectar a daemon de usb
    d_addr.sin_family = AF_INET;
    d_addr.sin_port = htons(DAEM_PORT);
    d_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(d_addr.sin_zero), '\0', 8);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd != -1) {
        printf("Scoket server success\n");
        // addr.sin_family = AF_INET;
        // addr.sin_port = htons(PRIV_PORT);
        // addr.sin_addr.s_addr = INADDR_ANY;
        // memset(&(addr.sin_zero), '\0', 8);
        // int b = bind(sockfd, (struct sockaddr *) &addr, sizeof(struct sockaddr));
        // if (b == -1) {
        //     printf("Bind server error\n");
        //     exit(1);
        // } else {
        //     printf("Bind server succes\n");
        // }
        int c = connect(sockfd, (struct sockaddr *) &d_addr, sizeof(struct sockaddr));
        if (c != -1) {
            printf("Connect server-daemon success\n");
        } else {
            printf("Connect server-daemon error\n");
            exit(1);
        }
    } else {
        printf("Socket server error\n");
        exit(1);
    }
    //Fin de la implementación socket

    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                              &connection_handler, NULL, MHD_OPTION_END);

    if (NULL == daemon) {
        printf("Error en manejar MHD daemon.\n");
        return 1;
    }

    while (1) {
    }

    MHD_stop_daemon(daemon);
    return 0;
}