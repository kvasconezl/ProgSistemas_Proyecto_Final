struct udev_device* getHijo(struct udev* udev, struct udev_device* padre, const char* subsistema);
char * enumerar_disp_alm_masivo(struct udev* udev);
void estructuraToString(const struct mntent *fs);
void direccionDispositivo(const char *direccion_fisica);
void connectionHandler();
char* dispositivo(char *direccion_fisica);
char* parsearLista(char* solicitud);
int escrituraArchivo(char* direccion, char* nombre_archivo, int tamano, char* contenido);
