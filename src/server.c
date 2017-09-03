#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888

static int connection_handler(void *cls, struct MHD_Connection *connection,
                              const char *url, const char *method,
                              const char *version, const char *upload_data,
                              size_t *upload_data_size, void **con_cls) {
    char *page = "<html><body></body></html>";
    struct MHD_Response *response;
    int ret;

    // if (strcmp("GET", method) == 0) {
    //     printf("GET ");
    //     if (strcmp("/listar_dispositivos", url) == 0) {
    //         printf("/listar_dispositivos\n");
    //     } else if (strcmp("/leer_archivo", url) == 0) {
    //         printf("/leer_archivo\n");
    //     }
    // } else if (strcmp("POST", method) == 0) {
    //     printf("POST ");
    //     if (strcmp("/nombrar_dispositivo", url) == 0) {
    //         printf("/nombrar_dispositivo\n");
    //     } else if (strcmp("/escribir_archivo", url) == 0) {
    //         printf("/escribir_archivo\n");
    //     }
    // }

    response = MHD_create_response_from_buffer(strlen(page), (void *) page,
                                               MHD_RESPMEM_PERSISTENT);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);

    MHD_destroy_response(response);

    return ret;
}

int main(void) {
    struct MHD_Daemon *daemon;

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