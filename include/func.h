#include <errno.h>
#include <libudev.h>
#include <locale.h>
#include <mntent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct udev_device *obtener_hijo(struct udev *udev, struct udev_device *padre, const char *subsistema);
void enumerar_disp_alm_masivo(struct udev *udev, int logsdaemon);
void presentar_estructuraMNTENT(const struct mntent *fs);
void direccionDispositivo(const char *direccion_fisica);