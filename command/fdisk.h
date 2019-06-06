#ifndef FDISK_H
#define FDISK_H

#include "../var/globals.h"
#include "../fileManager/manager.h"

void deletePart()
{

}

void modifyPart()
{

}

void createPart()
{
    Partition newPart;
    int best = 0;
    int worst = 0;
    int first = -1;
    int free_part = -1;
    int has_extended = 0;
}

void exec_fdisk()
{
    if (strlen(values.path) < 0 || strlen(values.name) < 0)
    {
        printf(ANSI_COLOR_RED "[e] Path y/o name son requeridos\n" ANSI_COLOR_RESET);
        return;
    }

    if (!existDisk())
    {
        printf(ANSI_COLOR_RED "[e] El disco %s no existe\n" ANSI_COLOR_RESET, values.path);
        return;
    }

    if (values.unit == 'k')
    {
        values.size *= 1024;
        values.size *= 1024;
    }
    else if (values.unit == 'm')
    {
        values.size *= 1024 * 1024;
        values.size *= 1024 * 1024;
    }
    else
    {
        values.size *= 1024;
        values.size *= 1024;
    }

    if (strlen(values.del) < 0) deletePart();
    else if (values.add != 0) modifyPart();
    else createPart();
}

#endif FDISK_H