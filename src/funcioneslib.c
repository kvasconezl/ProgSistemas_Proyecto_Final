#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <mntent.h>
#include <errno.h>
#include <string.h>
#define BUFFDISPOSITIVOS 10000
#define BUFLEN 1024

char* lecturaArchivo(char* direccion, char* nombre_archivo){
	FILE *file;
	int variable;
	char result[1000];
	char* texto_final=NULL;
	sprintf(result,"%s/%s", direccion,nombre_archivo);
	file = fopen(result,"r");
	if (file == NULL){
		printf("\nError en abrir el archivo\n\n");
	}else{
		while((variable = fgetc(file)) != EOF) sprintf(texto_final,"%s%c",texto_final,variable);
	}
	fclose(file);
	return texto_final;
}

int escrituraArchivo(char* direccion, char* archivo, int tamano, char* contenido){
	printf("%s-%s\n",direccion, archivo );
	char *rfinal = malloc((strlen(direccion)+strlen(archivo)+1)* sizeof(char*));
	memset(rfinal,0,strlen(direccion)+strlen(archivo)+1);
	sprintf(rfinal,"%s/%s", direccion,archivo);
	printf("\n%s\n",rfinal );
	FILE* file;
	file = fopen(rfinal, "w");
	if(file<=0){
		return 0;
	}
	fputs(contenido, file);
	fclose(file);
	return 1;
}

void estructuraToString(const struct mntent *fs){
	printf("nodo :%s \n direccion logica :%s \n %s \n %s \n %d \n %d\n",
		   fs->mnt_fsname,  
		   fs->mnt_dir,    
		   fs->mnt_type,	
		   fs->mnt_opts,	
		   fs->mnt_freq,	
		   fs->mnt_passno);	
}

struct udev_device * getHijo(struct udev* udev, struct udev_device* padre, const char* subsistema){
	struct udev_device* hijo = NULL;
	struct udev_enumerate *enumerar = udev_enumerate_new(udev);

	udev_enumerate_add_match_parent(enumerar, padre);
	udev_enumerate_add_match_subsystem(enumerar, subsistema);
	udev_enumerate_scan_devices(enumerar);

	struct udev_list_entry *dispositivos = udev_enumerate_get_list_entry(enumerar);
	struct udev_list_entry *entrada;
	udev_list_entry_foreach(entrada, dispositivos){
		const char *ruta = udev_list_entry_get_name(entrada);
		hijo = udev_device_new_from_syspath(udev, ruta);
		break;
	}
	udev_enumerate_unref(enumerar);
	return hijo;
}

const char* direccionDispositivo(const char *direccion_fisica){
	FILE *fp;
	struct mntent *fs;
	fp = setmntent("/etc/mtab", "r");
	if (fp == NULL) {
		return "\"str_error\":\"ERROR: Al intentar abrir el fichero: /etc/mtab que contiene la direccion logico de los disp USB\"";
	}
	while ((fs = getmntent(fp)) != NULL){
		if(strstr(fs->mnt_fsname,direccion_fisica)>0){
			endmntent(fp);
			return fs->mnt_dir;
		}
	}
	endmntent(fp);
	return  "no se encuentra montado dicho dispositivo";
}

char* enumerar_disp_alm_masivo(struct udev* udev){

	struct udev_enumerate* enumerar = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerar, "scsi");
	udev_enumerate_add_match_property(enumerar, "DEVTYPE", "scsi_device");
	udev_enumerate_scan_devices(enumerar);
	struct udev_list_entry *dispositivos = udev_enumerate_get_list_entry(enumerar);
	struct udev_list_entry *entrada;

	char *lista = (char *)malloc(BUFFDISPOSITIVOS);
	int variable=0;
	udev_list_entry_foreach(entrada, dispositivos) {
		char *concat_str = (char *)malloc(BUFFDISPOSITIVOS);
		const char* ruta = udev_list_entry_get_name(entrada);
		struct udev_device* scsi = udev_device_new_from_syspath(udev, ruta);
		struct udev_device* block = getHijo(udev, scsi, "block");
		struct udev_device* scsi_disk = getHijo(udev, scsi, "scsi_disk");
		struct udev_device* usb= udev_device_get_parent_with_subsystem_devtype(scsi, "usb", "usb_device");
		if (block && scsi_disk && usb){
			const char *nodo=udev_device_get_devnode(block);
			const char * validarerror=direccionDispositivo(nodo);
			if(strstr(validarerror, "str_error")!=NULL ){
				return (char *)validarerror;
			}
			variable=sprintf(concat_str, "{\"nodo\":\"%s\", \"nombre\":\" \",\"montaje\":\"%s\",\"Vendor:idProduct\":\"%s:%s\",\"scsi\":\"%s\"}",
							 nodo,
							 direccionDispositivo(nodo),
							 udev_device_get_sysattr_value(usb, "idVendor"),
							 udev_device_get_sysattr_value(usb, "idProduct"),
							 udev_device_get_sysattr_value(scsi, "vendor"));
			if(strstr(lista, "nodo")!=NULL){
				char *copia = (char *)malloc(BUFFDISPOSITIVOS);
				sprintf(copia, "%s",lista);
				sprintf(lista, "%s,%s",copia,concat_str);
			}else{
				sprintf(lista, "%s",concat_str);
			}
		}
		if (block) udev_device_unref(block);
		if (scsi_disk) udev_device_unref(scsi_disk);
		udev_device_unref(scsi);

	}
	if(variable==0) lista=" ";
	udev_enumerate_unref(enumerar);
	return lista;

}

char* dispositivo(char *direccion_fisica){
	FILE *fp;
	struct mntent *fs;
	fp = setmntent("/etc/mtab", "r");
	if (fp == NULL) {
		return "\"str_error\":\"ERROR: Al intentar abrir el fichero: /etc/mtab que contiene la direccion logico de los disp USB\"";
	}
	while ((fs = getmntent(fp)) != NULL){
		if(strstr(fs->mnt_fsname,direccion_fisica)>0){
			endmntent(fp);
			return(char*) fs->mnt_dir;
		}
	}
	endmntent(fp);
	return  "Este dispositivo no se encuentra montado";
}

char* parsearLista(char* solicitud){
	char* lista[6];
	const char delimitadores[2] = "|";
	char *token;
	token = strtok(solicitud, delimitadores);
	int i=0;
	while( token != NULL ) {
		lista[i]=token;
		printf("%d:%s\n",i,token );
		i++;
		token = strtok(NULL, delimitadores);
	}
	escrituraArchivo(lista[5],lista[0],atoi(lista[1]),lista[3]);
	char *concat_str = malloc(BUFLEN* sizeof(char*));
	sprintf(concat_str, "{\"solicitud\":\"%s\", \"nombre\":\" %s\",\"nombre_archivo\":\"%s\",",lista[2],lista[4],lista[0]);
	return concat_str;
}