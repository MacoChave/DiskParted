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

    updateEBR(values.path, ebr, start);
    printf(ANSI_COLOR_GREEN "[i] Se creó la partición extendida %s\n" ANSI_COLOR_RESET, ebr.ebr_name);
}

void createLogicalPartition (Partition part, char fit)
{
    int idx = -1;
    EBR ebr = getEBR(values.path, part.part_start);
    if (ebr.part_size < 0)
    {
        printf(ANSI_COLOR_RED "[e] Se ha perdido el ebr de la partición lógica\n" ANSI_COLOR_RESET);
        return;
    }
    getSpaceLogicalDetail(values.path, ebr, part.part_start + part.part_size);
    if (fit == 'f')
        idx = getFirstAdjustPart();
    else if (fit == 'b')
    {
        getBestAdjustPart();
        idx = getFirstAdjustPart();
    }
    else if (fit == 'w')
    {
        getWorstAdjustPart();
        idx = getFirstAdjustPart();
    }

    if ((spaces[idx].start + values.size + sizeof(EBR)) > (part.part_start + part.part_size))
    {
        printf(ANSI_COLOR_RED "[e] No hay espacio en la partición\n" ANSI_COLOR_RESET);
        return;
    }
    if (spaces[idx].start == 0)
    {
        printf(ANSI_COLOR_RED "[e] No hay espacio en la partición\n" ANSI_COLOR_RESET);
        return;
    }
    for (int i = 0; i < 50; i++)
    {
        if (spaces[idx].space > 0)
        {
            EBR ebr = getEBR(values.path, spaces[idx].start);
            if (strcmp(ebr.ebr_name, values.name) == 0)
            {
                printf(ANSI_COLOR_RED "[e] Ya existe una partición %s\n" ANSI_COLOR_RESET, values.name);
                return;
            }
        }
    }
    
    EBR new_ebr;
    memset(new_ebr.ebr_name, 0, 16);
    strcpy(new_ebr.ebr_name, values.name);
    new_ebr.part_fit = values.fit;
    new_ebr.part_next = -1;
    new_ebr.part_size = values.size;
    new_ebr.part_start = spaces[idx].start;
    new_ebr.part_status = '0';
    
    EBR new_blank;
    memset(new_blank.ebr_name, 0, 16);
    new_blank.part_fit = '0';
    new_blank.part_next = -1;
    new_blank.part_size = 0;
    new_blank.part_start = 0;
    new_blank.part_status = '0';

    if (spaces[idx].prev > 0)
    {
        EBR prev = getEBR(values.path, spaces[idx].prev);
        
        if (prev.part_next > 0 && prev.part_next != new_ebr.part_start)
            new_ebr.part_next = prev.part_next;
        
        prev.part_next = spaces[idx].start;
        updateEBR(values.path, prev, prev.part_start);
    }

    if (spaces[idx].next < 0)
    {
        new_blank.part_start = spaces[idx].start + values.size + sizeof(EBR);
        new_ebr.part_next = new_blank.part_start;
        updateEBR(values.path, new_blank, new_blank.part_start);
    }
    else
        new_ebr.part_next = spaces[idx].next;
    updateEBR(values.path, new_ebr, new_ebr.part_start);
    printf(ANSI_COLOR_GREEN "[i] Se creó la partición lógica %s\n" ANSI_COLOR_RESET, new_ebr.ebr_name);
}

void modifyPart()
{
    clearSpaceDisk();
    int i = getDiskByPath(values.path);
    int j = getPartByName(values.name, i);

    if (j != _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    MBR mbr = getMBR(values.path);

    for (int i = 0; i < 4; i++)
    {
        if (mbr.partitions[i].part_size > 0)
        {
            if (strcmp(mbr.partitions[i].part_name, values.name) == 0)
            {
                int pto = mbr.partitions[i].part_start + mbr.partitions[i].part_size + values.add;
                if (pto > mbr.partitions[i].part_start)
                {
                    if (i < 3)
                    {
                        if (mbr.partitions[i + 1].part_start > 0)
                        {
                            if (pto > mbr.partitions[i + 1].part_start)
                            {
                                printf(ANSI_COLOR_RED "[e] No hay espacio suficiente para partición %s\n" ANSI_COLOR_RESET, values.name);
                                return;
                            }
                        }
                        else
                        {
                            if (pto > mbr.size)
                            {
                                printf(ANSI_COLOR_RED "[e] No hay espacio suficiente para partición %s\n" ANSI_COLOR_RESET, values.name);
                                return;
                            }
                        }
                    }
                    else
                    {
                        if (pto > mbr.size)
                        {
                            printf(ANSI_COLOR_RED "[e] No hay espacio suficiente para partición %s\n" ANSI_COLOR_RESET, values.name);
                            return;
                        }
                    }

                    mbr.partitions[i].part_size += values.add;
                    updateMBR(values.path, mbr);
                    printf(ANSI_COLOR_GREEN "[i] Se ha agregado %d a la partición %s\n" ANSI_COLOR_RESET, values.add, values.name);
                    return;
                }
                else
                {
                    printf(ANSI_COLOR_RED "[e] No quedará espacio en partición %s\n" ANSI_COLOR_RESET, values.name);
                    return;
                }
            }
        }
    }
    
    int ext = getNumberExtendedPart(mbr.partitions);
    EBR ebr = getEBR(values.path, mbr.partitions[ext].part_start);
    getSpaceLogicalDetail(values.path, ebr, mbr.partitions[ext].part_start + mbr.partitions[ext].part_size);

    for (int i = 0; i < 50; i++)
    {
        if (spaces[i].type == 'l')
        {
            ebr = getEBR(values.path, spaces[i].start);
            if (strcmp(ebr.ebr_name, values.name) == 0)
            {
                int pto = ebr.part_start + ebr.part_size + values.add;
                if (pto < spaces[i].start + sizeof(EBR))
                {
                    printf(ANSI_COLOR_RED "[e] No quedará espacio en partición %s\n" ANSI_COLOR_RESET, values.name);
                    return;
                }
                else
                {
                    EBR next = getEBR(values.path, spaces[i].next);
                    if (next.part_size > 0)
                    {
                        if (pto > ebr.part_start)
                        {
                            printf(ANSI_COLOR_RED "[e] No hay espacio suficiente para partición %s\n" ANSI_COLOR_RESET, values.name);
                            return;
                        }
                    }
                    else
                    {
                        if (pto > (mbr.partitions[ext].part_start + mbr.partitions[ext].part_size))
                        {
                            printf(ANSI_COLOR_RED "[e] No hay espacio suficiente para partición %s\n" ANSI_COLOR_RESET, values.name);
                            return;
                        }
                        next.part_start = pto;
                        ebr.part_next = pto;
                        updateEBR(values.path, next, next.part_start);
                    }

                    ebr.part_size += values.add;
                    updateEBR(values.path, ebr, ebr.part_start);
                    printf(ANSI_COLOR_GREEN "[i] Se ha agregado %d a la partición %s\n" ANSI_COLOR_RESET, values.add, values.name);
                    return;
                }
            }
        }
    }
}

void deletePart()
{
    clearSpaceDisk();
    int i = getDiskByPath(values.path);
    int j = getPartByName(values.name, i);

    if (j != _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    MBR mbr = getMBR(values.path);

    for (int i = 0; i < 4; i++)
    {
        if (mbr.partitions[i].part_size > 0)
        {
            if (strcmp(mbr.partitions[i].part_name, values.name) == 0)
            {
                printf(ANSI_COLOR_YELLOW "[w] ¿Desea eliminar la partición %s? y/n\n" ANSI_COLOR_RESET, values.name);
                char conf[999] = {0};
                fgets(conf, 999, stdin);
                if (conf[0] == 'y' || conf[0] == 'Y')
                {
                    if (strcmp(values.del, "full") == 0)
                        clearPartDisk(values.path, mbr.partitions[i].part_start, mbr.partitions[i].part_size);

                    mbr.partitions[i].part_status = '0';
                    mbr.partitions[i].part_type = '0';
                    mbr.partitions[i].part_fit = '0';
                    mbr.partitions[i].part_start = 0;
                    mbr.partitions[i].part_size = 0;
                    memset(mbr.partitions[i].part_name, 0, 16);
                    mbr = sortMBR(mbr);
                    updateMBR(values.path, mbr);
                    printf(ANSI_COLOR_GREEN "[i] Partición %s eliminada\n" ANSI_COLOR_RESET, values.name);
                }
                return;
            }
        }
    }
    
    int ext = getNumberExtendedPart(mbr.partitions);
    EBR ebr = getEBR(values.path, mbr.partitions[ext].part_start);
    getSpaceLogicalDetail(values.path, ebr, mbr.partitions[ext].part_start + mbr.partitions[ext].part_size);

    for (int i = 0; i < 50; i++)
    {
        if (spaces[i].type == 'l')
        {
            ebr = getEBR(values.path, spaces[i].start);
            if (strcmp(ebr.ebr_name, values.name) == 0)
            {
                printf(ANSI_COLOR_YELLOW "[w] ¿Desea eliminar la partición %s? y/n\n" ANSI_COLOR_RESET, values.name);
                char conf[999] = {0};
                fgets(conf, 999, stdin);
                if (conf[0] == 'y' || conf[0] == 'Y')
                {
                    if (spaces[i].prev > 0)
                    {
                        EBR prev = getEBR(values.path, spaces[i].prev);
                        EBR next = getEBR(values.path, spaces[i].next);
                        if (next.part_size > 0)
                            prev.part_next = next.part_start;
                        else
                            next.part_start = ebr.part_start;
                        
                        updateEBR(values.path, prev, prev.part_start);
                        clearPartDisk(values.path, ebr.part_start, ebr.part_size + sizeof(EBR));
                        updateEBR(values.path, next, next.part_start);
                    }
                    else {
                        EBR next = getEBR(values.path, spaces[i].next);
                        if (next.part_size > 0)
                        {
                            ebr.part_size = 0;
                            memset(ebr.ebr_name, 0, 16);
                        }
                        else
                            next.part_start = ebr.part_start;
                        
                        updateEBR(values.path, ebr, ebr.part_start);
                        updateEBR(values.path, next, next.part_start);
                    }

                    printf(ANSI_COLOR_GREEN "[i] Partición %s eliminada\n" ANSI_COLOR_RESET, values.name);
                }
                return;
            }
        }
    }
}

void createPart()
{
    Partition newPart;
    int idx = -1;
    int ext = -1;
    MBR mbr = getMBR(values.path);
    ext = getNumberExtendedPart(mbr.partitions);

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
        if (ext != _ERROR_)
        {
            printf(ANSI_COLOR_RED "[e] Ya se encuentra una partición extendida\n" ANSI_COLOR_RESET);
            return;
        }
    }
    if (values.type == 'l')
    {
        if (ext == _ERROR_)
        {
            printf(ANSI_COLOR_RED "[e] No hay partición extendida %s\n" ANSI_COLOR_RESET, values.name);
            return;
        }
        else
        {
            createLogicalPartition(mbr.partitions[ext], mbr.fit);
            return;
        }
    }
    getSpacePrimaryDetail(mbr.partitions, mbr.size);
    if (mbr.fit == 'f')
        idx = getFirstAdjustPart();
    else if (mbr.fit == 'b')
    {
        getBestAdjustPart();
        idx = getFirstAdjustPart();
    }
    else if (mbr.fit == 'w')
    {
        getWorstAdjustPart();
        idx = getFirstAdjustPart();
    }
    
    for (int i = 0; i < 4; i++)
    {
        if (mbr.partitions[i].part_size <= 0)
        {
            Partition part;
            part.part_fit = values.fit;
            strcpy(part.part_name, values.name);
            part.part_size = values.size;
            part.part_start = spaces[idx].start;
            part.part_status = '0';
            part.part_type = values.type;
            mbr.partitions[i] = part;
            mbr = sortMBR(mbr);
            updateMBR(values.path, mbr);
            if (values.type == 'e')
                createExtendedPartition(part.part_start);
            printf(ANSI_COLOR_GREEN "[i] Partición %s : %d creada en disco %s\n" ANSI_COLOR_RESET, part.part_name, part.part_size, values.path);
            return;
        }
    }
    printf(ANSI_COLOR_RED "[e] No hay espacio en tabla de particiones\n" ANSI_COLOR_RESET);
}

void exec_fdisk()
{
    clearSpaceDisk();
    if (strlen(values.path) < 0 || strlen(values.name) < 0)
    {
        printf(ANSI_COLOR_RED "[e] Path y/o name son requeridos\n" ANSI_COLOR_RESET);
        return;
    }

    if (!existDisk(values.path))
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

    if (strcmp(values.del, "full") == 0 || strcmp(values.del, "fast") == 0) deletePart();
    else if (values.add != 0) modifyPart();
    else createPart();

    clearSpaceDisk();
}

#endif