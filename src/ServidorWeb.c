#include <sys/types.h>
#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#else
#include <winsock2.h>
#endif
#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/jsmn.h"
#include <sys/stat.h>
#include <stddef.h>             /* for offsetof */
#include <unistd.h>             /* for convenience */
#include <netdb.h>
#include <errno.h>
#include <syslog.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#define BUFLEN 1024
#define MAXSLEEP 24
#define BUFFERING 10000

struct listaUsb{
    char* nombre;
    char* nodo;
    char* montaje;
    char* sci;
    char* VendoridProduct;
};

struct colaJson{
    char* info;
    char* stringjson;
};

struct nombreUsb{
    char* nombre;
    char* direccion_fisica;
    char* direccion_logica;
};

struct nombreUsb* nombrados[5];
int elementos=0;
struct listaUsb* usblista[5];
int usbelementos=0;

int connect_retry( int domain, int type, int protocol,  const struct sockaddr *addr, socklen_t alen){
  int numsec, fd;
  for (numsec = 1; numsec <= MAXSLEEP; numsec++) {
    if (( fd = socket( domain, type, protocol)) < 0)
      return(-1);
    if (connect( fd, addr, alen) == 0) {
      return(fd);
    }
    close(fd);
    sleep(1);
  }
  return(-1);
}

char* clienteNew(char *solicitud){
  int sockfd,filefd;
  int puerto =8888;
  int n=0;
  struct sockaddr_in direccion_cliente;
  memset(&direccion_cliente, 0, sizeof(direccion_cliente));
  direccion_cliente.sin_family = AF_INET;   //IPv4
  direccion_cliente.sin_port = htons(puerto);
  direccion_cliente.sin_addr.s_addr = inet_addr("127.0.0.1") ;
  if (( sockfd = connect_retry( direccion_cliente.sin_family, SOCK_STREAM, 0, (struct sockaddr *)&direccion_cliente, sizeof(direccion_cliente))) < 0) {
    printf("Falla de conexion. \n\n");
    return "\"str_error\":\"ERROR:Falla en la conexion.\n";
  }
  if(strstr(solicitud, "escribir_archivo")!=NULL){
    send(sockfd,"escribir_archivo",BUFLEN,0);
    sleep(1);
    send(sockfd,solicitud,strlen(solicitud),0);
    printf("Solicitud enviada proceso daemon:\n ");
    printf("Procesando.....\n");
    sleep(1);
    char *file = malloc(BUFLEN*sizeof(char *));
    memset(file,0,BUFLEN);
    if((n=recv(sockfd, file,BUFLEN,0))>0){
      if (strstr(file, "ERROR") != NULL) {
        printf("ERROR: Respuesta del daemon\n");
        close(sockfd);
        return "\"str_error\":\"ERROR: Respuesta del daemon\"\n";
      }
      printf("Respuesta:\n %s\n",file);
      close(sockfd);
      return file;
    }
  }else{
    send(sockfd,solicitud,BUFLEN,0);
    printf("Solicitud enviada proceso daemon:\n %s \n",solicitud);
    printf("procesando respuesta del daemon\n");
    char *file = malloc(BUFFERING*sizeof(char *));
    memset(file,0,BUFFERING);
    if((n=recv(sockfd, file, BUFFERING,0))>0){
      if (strstr(file, "ERROR") != NULL) {
        printf("ERROR: al recibir respuesta del daemon\n");
        close(sockfd);
        return "\"str_error\":\"ERROR:al recibir respuesta del daemon\"\n";
      }
      printf("respuesta del daemonUSB:\n %s\n",file);
      close(sockfd);
      return file;
    }
  }
  close(sockfd);
  return "\"str_error\":\"ERROR:no se recibio respuesta del daemon\"\n";
}

static int enviarRespuesta(struct MHD_Connection *connection, const char *page, int statuscodigo){
  int ret;
  struct MHD_Response *response;
  response =MHD_create_response_from_buffer (strlen (page), (void *) page,MHD_RESPMEM_PERSISTENT);
  if (!response) return MHD_NO;
  MHD_add_response_header(response,"Content-Type","application/json");
  ret = MHD_queue_response (connection, statuscodigo, response);
  MHD_destroy_response (response);
  return ret;
}


static int jsonLimpio(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}



static int iterarEncabezado (void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
  printf ("Encabezado %s: %s\n", key, value);
  return MHD_YES;
}



void visualizarLista(struct listaUsb *lista[]){
  printf("\n HISTORIAL VIRTUAL SERVIDOR DE DISPOSITIVO . \n");
  if(usbelementos==0) {
    printf("Lista vacia.No hay dispositivo USB . \n");
    return;
  }
  for (int i = 0; i < usbelementos; i++) {
    printf("USB N %d | nombre:%s,nodo:%s,montaje:%s,sci:%s,VendoridProduct :%s \n", i+1,lista[i]->nombre,lista[i]->nodo,lista[i]->montaje,lista[i]->sci,lista[i]->VendoridProduct);
  }
}

void procesandolistaUSB(const char *upload_data) {
  char *elementolista=malloc(sizeof(char)*(60));
  int r,i;
  jsmn_parser p;
  jsmntok_t t[128];
  jsmn_init(&p);
  char* nodo=malloc(sizeof(char)*(10));
  char *nombre=malloc(sizeof(char)*(50));
  char *montaje=malloc(sizeof(char)*(50));
  char *sci=malloc(sizeof(char)*(50));
  char *VendoridProduct = malloc(sizeof(char)*(50));
  char* upload_datas = malloc(sizeof(char)*(strlen( upload_data)-2));
  memset(upload_datas,0,strlen( upload_data)-2);
  int j=0;
  for(int i=0;i<strlen(upload_data);i++){
    if(upload_data[i]=='{' && j==0){
      while(upload_data[i]!='}'){
        upload_datas[j]=upload_data[i];
        i++;
        j++;
      }
      upload_datas[j]=upload_data[i];
    }
  }
  char * re=malloc((j-1)*sizeof(char *));
  memset(re,0,j-1);
  re=upload_datas;
  r = jsmn_parse(&p,  upload_data, strlen(upload_data), t, 128);
  if (r < 0) {
    return ;
  }
  if (r < 1 || t[0].type != JSMN_OBJECT) {
    return ;
  }
  for (i = 1; i < r; i++) {
    if (jsonLimpio( upload_data, &t[i], "nombre") == 0) {
      sprintf(nombre,"%.*s",t[i+1].end-t[i+1].start, upload_datas + t[i+1].start);
      i++;
    }
    if (jsonLimpio( upload_data, &t[i], "nodo") == 0) {
      sprintf(nodo,"%.*s",t[i+1].end-t[i+1].start, upload_datas + t[i+1].start);
      i++;
    }
    if (jsonLimpio( upload_data, &t[i], "montaje") == 0) {
      sprintf(montaje,"%.*s",t[i+1].end-t[i+1].start, upload_datas + t[i+1].start);
      i++;
    }
    if (jsonLimpio( upload_data, &t[i], "scsi") == 0) {
      sprintf(sci,"%.*s",t[i+1].end-t[i+1].start, upload_datas + t[i+1].start);
      i++;
    }
    if (jsonLimpio( upload_data, &t[i], "Vendor:idProduct") == 0) {
      sprintf(VendoridProduct,"%.*s",t[i+1].end-t[i+1].start, upload_datas + t[i+1].start);
      i++;
    }
  }
  struct listaUsb *usb2=malloc(sizeof(struct listaUsb));
  usb2->nombre=nombre;
  usb2->nodo=nodo;
  usb2->montaje=montaje;
  usb2->sci=sci;
  usb2->VendoridProduct=VendoridProduct;
  usblista[usbelementos]=usb2;
  usbelementos++;
  return;
}

void parsearLista(char* listausb){
  char* usb1=malloc(BUFLEN*sizeof(char*));
  if(strstr(listausb,"},")>0){
    const char delimitadores[2] = "}";
    char *token;
    token = strtok(listausb, delimitadores);
    int i=0;
    while( token != NULL ) {
      sprintf(usb1,"%s}",token);
      if(i>0){
        procesandolistaUSB(usb1+1);
        printf("%s\n", usb1+1);
      }else{
        procesandolistaUSB(usb1);
        printf("%s\n", usb1);
        i++;
      }
      token = strtok(NULL, delimitadores);

    }
  }else{
    procesandolistaUSB(listausb);
  }
}



int responderSolicitud (void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version,
                        const char *upload_data, size_t *upload_data_size, void **con_cls){

  char* jsonRespuesta = malloc(BUFFERING*sizeof(char *));
  memset(jsonRespuesta,0,BUFFERING);
  char* solicitudR = malloc(BUFFERING*sizeof(char *));
  memset(solicitudR,0,BUFFERING);


  if (0 == strcmp (method,MHD_HTTP_METHOD_GET)  && !strncasecmp(url, "/listar_dispositivos", 19)){
    sprintf(solicitudR, "%s-%s",method,"listar_dispositivos");
    printf ("\nNueva  %s solicitud en  %s con  version %s \n", method, url, version);
    MHD_get_connection_values (connection, MHD_HEADER_KIND, iterarEncabezado,NULL);
    char *response = clienteNew(solicitudR);
    if(strstr(response, "str_error")!=NULL ){
      sprintf(jsonRespuesta,"{\"solicitud\": \"listar_dispositivos\", \n"
              "\"status\": \"-1 \", %s}",response);
      return enviarRespuesta (connection, jsonRespuesta,400);
    }else{
      if(strlen(response)<=1){
        sprintf(jsonRespuesta,"{\"solicitud\": \"listar_dispositivos\", \"dispositivos\": [%s ], \n"
                "\"status\": \"0\", \"str_error\" :\"no existen dispositivos actualmente conectados\" }",response);
        return enviarRespuesta (connection, jsonRespuesta, MHD_HTTP_OK);
      }else{
        sprintf(jsonRespuesta,"{\"solicitud\": \"listar_dispositivos\", \"dispositivos\": [%s ], \n"
                "\"status\": \"0\", \"str_error\" : 0}",response);
        parsearLista(response);
        visualizarLista(usblista);
        return enviarRespuesta (connection, jsonRespuesta, MHD_HTTP_OK);
      }
    }
  }
  else{

    printf ("\nNueva  %s solicitud en  %s con  version %s \n", method, url, version);
    MHD_get_connection_values (connection, MHD_HEADER_KIND, iterarEncabezado,NULL);
    char * response="\"str_error\":\"Eror en soliciud";
    sprintf(jsonRespuesta,"{\"solicitud\": \"%s\", \n"
            "\"status\": \" -1\", %s }",url,response);
    int respuesta = enviarRespuesta (connection, jsonRespuesta,404);
    printf("%d \n",respuesta) ;
    return respuesta;
  }
}

int main (int argc, char *argv[]){
  if(argc != 2){
    printf("Uso: ./bin/ServidorWeb <puerto>\n");
    exit(-1);
  }
  int puerto = atoi(argv[1]);
  struct MHD_Daemon *daemon;
  daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY, puerto, NULL, NULL, &responderSolicitud, NULL,MHD_OPTION_END);
  if (NULL == daemon) return 1;

  (void) getchar ();

  MHD_stop_daemon (daemon);
  return 0;
}
