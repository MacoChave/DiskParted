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

    if (!existDisk())
    {
        printf(ANSI_COLOR_RED "[e] No se encontró un disco en %s\n" ANSI_COLOR_RESET, values.path);
        return;
    }

    removeDisk();
    printf(ANSI_COLOR_GREEN "[i] Disco %d eliminado\n" ANSI_COLOR_RESET, values.path);
}

#endif