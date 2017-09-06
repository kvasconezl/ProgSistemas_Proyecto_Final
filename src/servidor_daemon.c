#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <libudev.h>
#include <mntent.h>
#include "Monitor.h"
#define BUFLEN 1024
#define BUFFERING 10000
#define QLEN 50

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif
int clfd;
int filefd;
int sockfd;
int fd;


int server(int type, const struct sockaddr *addr, socklen_t alen, int qlen){
	int err = 0;
	if((fd = socket(addr->sa_family, type, 0)) < 0)
		return -1;
	if(bind(fd, addr, alen) < 0)
		goto errout;
	if(type == SOCK_STREAM || type == SOCK_SEQPACKET){
		if(listen(fd, QLEN) < 0)
			goto errout;
	}
	return fd;
	errout:
	err = errno;
	close(fd);
	errno = err;
	return (-1);
}

void connectionHandler(){
	int  n,puerto = 8888;
	char *host;
	if (( n = sysconf(_SC_HOST_NAME_MAX)) < 0) n = HOST_NAME_MAX;
	if ((host = malloc(n)) == NULL) printf(" malloc error");
	if (gethostname( host, n) < 0)
		printf(" gethostname error");
	struct sockaddr_in direccion_servidor;
	memset(&direccion_servidor, 0, sizeof(direccion_servidor));
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_port = htons(puerto);
	direccion_servidor.sin_addr.s_addr = inet_addr("127.0.0.1") ;
	if( (sockfd = server(SOCK_STREAM, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor), 1000)) < 0){
		printf("Ya existe un proceso en ejecucion\n");
	}

	while(1){
		if (( clfd = accept( sockfd, NULL, NULL)) < 0) {
			close(clfd);
			continue;
		}
		char *solicitud = malloc(BUFLEN*sizeof(char *));
		recv(clfd, solicitud, BUFLEN, 0);
		printf("%s\n", solicitud);
		if ((strstr(solicitud, "GET") != NULL) && (strstr(solicitud, "listar_dispositivos") != NULL)) {
			struct udev *udeva;
			udeva = udev_new();
			char* lista=enumerar_disp_alm_masivo(udeva);
			send(clfd,lista,strlen(lista),0);
			close(clfd);
		}
		close(clfd);
	}
}

