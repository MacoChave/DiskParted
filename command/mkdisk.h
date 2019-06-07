#ifndef MKDISK_H
#define MKDISK_H

#include "../var/globals.h"
#include "../fileManager/manager.h"

void exec_mkdisk()
{
    MBR mbr;
    if (strlen(values.path) <= 0 || values.size < 1)
    {
        printf(ANSI_COLOR_RED "[e] Path y/o size requerido\n" ANSI_COLOR_RESET);
        return;
    }

    if (values.unit == 'k')
        values.size = values.size * 1024;
    else if (values.unit == 'm')
        values.size = values.size * 1024 * 1024;
    else
        values.size = values.size * 1024;
    
    if (!createDirectory(values.path))
    {
        printf(ANSI_COLOR_RED "[e] No se puede crear el directorio %s\n" ANSI_COLOR_RESET, values.path);
        return;
    }

    if (existDisk(values.path))
    {
        printf(ANSI_COLOR_RED "[e] El disco ya existe\n" ANSI_COLOR_RESET);
        return;
    }

    if (!createDisk(values.path))
    {
        printf(ANSI_COLOR_RED "[e] No se pudo crear el disco\n" ANSI_COLOR_RESET);
        return;
    }

    mbr.mbr_disk_signature = 2010;
    mbr.size = values.size;
    mbr.fit = 'f';
    for (int i = 0; i < 4; i++)
    {
        mbr.partitions[i].part_status ='0';
        mbr.partitions[i].part_type = 'p';
        mbr.partitions[i].part_fit = 'f';
        mbr.partitions[i].part_start = 0;
        mbr.partitions[i].part_size = 0;
        memset(mbr.partitions[i].part_name, 0, 16);
    }

    time_t currentDate = time(NULL);
    struct tm * date = localtime(&currentDate);
    strftime(mbr.mbr_creation, sizeof(mbr.mbr_creation) - 1, "%d/%m/%y %H:%M", date);

    updateMBR(values.path, mbr);
    printf(ANSI_COLOR_GREEN "[i] Disco de %d bytes creado en %s\n" ANSI_COLOR_RESET, values.size, values.path);
}

#endif