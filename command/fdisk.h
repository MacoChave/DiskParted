#ifndef FDISK_H
#define FDISK_H

#include "../var/globals.h"
#include "../fileManager/mpartition.h"

void createExtendedPartition (int start)
{
    EBR ebr;
    memset(ebr.ebr_name, 0, 16);
    ebr.part_fit = '0';
    ebr.part_next = -1;
    ebr.part_size = 0;
    ebr.part_start = start;
    ebr.part_status = '0';

    updateEBR(ebr, start);
}

void deletePart()
{

}

void modifyPart()
{

}

void createPart()
{
    Partition newPart;
    int start = -1;
    int has_extended = 0;
    clearSpaceDisk();
    MBR mbr = getMBR();

    if (mbr.size == 0)
    {
        printf(ANSI_COLOR_RED "[e] No se encontró el disco en %s\n" ANSI_COLOR_RESET, values.path);
        return;
    }

    if (values.size < 0)
    {
        printf(ANSI_COLOR_RED "[e] Tamaño debe ser positivo\n" ANSI_COLOR_RESET);
        return;
    }

    if (getPartNumber(mbr.partitions) != _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] Ya existe la partición %s\n" ANSI_COLOR_RESET, values.name);
        return;
    }
    if (values.type == 'e')
    {
        if (getNumberExtendedPart(mbr.partitions) != _ERROR_)
        {
            printf(ANSI_COLOR_RED "[e] Ya se encuentra una partición extendida\n" ANSI_COLOR_RESET);
            return;
        }
    }
    getSpaceDetail(mbr.partitions, mbr.size);
    if (mbr.fit == 'f')
        start = getFirstAdjustPart();
    else if (mbr.fit == 'b')
    {
        getBestAdjustPart();
        start = getFirstAdjustPart();
    }
    else if (mbr.fit == 'w')
    {
        getWorstAdjustPart();
        start = getFirstAdjustPart();
    }
    
    for (int i = 0; i < 4; i++)
    {
        if (mbr.partitions[i].part_size <= 0)
        {
            Partition part;
            part.part_fit = values.fit;
            strcpy(part.part_name, values.name);
            part.part_size = values.size;
            part.part_start = start;
            part.part_status = '0';
            part.part_type = values.type;
            mbr.partitions[i] = part;
            mbr = sortMBR(mbr);
            updateMBR(mbr);
            if (values.type == 'e')
                createExtendedPartition(part.part_start);
            printf(ANSI_COLOR_GREEN "[i] Partición %s : %d creada en disco %s\n" ANSI_COLOR_RESET, part.part_name, part.part_size, values.path);
            break;
        }
    }    
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
        values.add *= 1024;
    }
    else if (values.unit == 'm')
    {
        values.size *= 1024 * 1024;
        values.add *= 1024 * 1024;
    }
    else
    {
        values.size *= 1024;
        values.add *= 1024;
    }

    if (strlen(values.del) < 0) deletePart();
    else if (values.add != 0) modifyPart();
    else createPart();
}

#endif