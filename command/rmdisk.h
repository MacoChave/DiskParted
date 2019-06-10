#ifndef RMDISK_H
#define RMDISK_H

#include "../var/globals.h"
#include "../fileManager/manager.h"

void exec_rmdisk()
{
    if (strlen(values.path) <= 0)
    {
        printf(ANSI_COLOR_RED "[e] Path requerido\n" ANSI_COLOR_RESET);
        return;
    }

    if (!existDisk(values.path))
    {
        printf(ANSI_COLOR_RED "[e] No se encontró un disco en %s\n" ANSI_COLOR_RESET, values.path);
        return;
    }

    int i = getDiskByPath(values.path);
    if (i != _ERROR_)
    {
        clearAllPartMounted(i);
        clearDiskMounted(i);
    }

    printf(ANSI_COLOR_YELLOW "[w] ¿Desea eliminar el disco %s? y/n\n" ANSI_COLOR_RESET, values.path);
    char conf[999] = {0};
    fgets(conf, 999, stdin);
    if (conf[0] == 'y' || conf[0] == 'Y')
    {
        removeDisk(values.path);
        printf(ANSI_COLOR_GREEN "[i] Disco %s eliminado\n" ANSI_COLOR_RESET, values.path);
    }
}

#endif