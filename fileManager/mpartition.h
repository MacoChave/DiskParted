#ifndef MPARTITION_H
#define MPARTITION_H

#include "../var/globals.h"
#include "../fileManager/manager.h"

SpaceDisk spaces[100];

void clearSpaceDisk()
{
    for (int i = 0; i < 100; i++)
    {
        spaces[i].start = 0;
        spaces[i].space = 0;
        spaces[i].type = '0';
    }    
}

int getPartNumber(Partition parts[])
{
    for (int i = 0; i < 4; i++)
    {
        if (parts[i].part_size == 0) continue;
        
        if (strcmp(parts[i].part_name, values.name) == 0)
            return i;
    }

    return _ERROR_;
}

int getNumberExtendedPart (Partition parts[])
{
    for (int i = 0; i < 4; i++)
    {
        if (parts[i].part_size <= 0) continue;
        
        if (parts[i].part_type == 'e')
            return i;
    }

    return _ERROR_;
}

void getBestAdjustPart()
{
    for (int i = 1; i < 100; i++)
    {
        if (spaces[i].start == 0) break;
        for (int j = 0; j < 99; j++)
        {
            if (spaces[j].start == 0) break;
            if (spaces[j].space > spaces[j + 1].space)
            {
                SpaceDisk aux = spaces[j];
                spaces[j] = spaces[j + 1];
                spaces[j + 1] = aux;
            }
        }
    }
}

void getWorstAdjustPart()
{
    for (int i = 1; i < 100; i++)
    {
        if (spaces[i].start == 0) break;
        for (int j = 0; j < 99; j++)
        {
            if (spaces[j].start == 0) break;
            if (spaces[j].space < spaces[j + 1].space)
            {
                SpaceDisk aux = spaces[j];
                spaces[j] = spaces[j + 1];
                spaces[j + 1] = aux;
            }
        }
    }
}

int getFirstAdjustPart()
{
    for (int i = 0; i < 100; i++)
    {
        if (spaces[i].type == 'f')
        {
            int free = spaces[i].space - values.size;
            if (free > 0) return spaces[i].start;
        }
    }    
}

void getSpaceDetail(Partition parts[], int size)
{
    int pivot = sizeof(MBR);
    int free = 0;
    int s = 0;
    int i = 0;
    while (pivot < size)
    {
        SpaceDisk space;
        if (parts[i].part_size == 0)
        {
            space.start = pivot;
            space.space = size - pivot;
            space.type = 'f';
            spaces[s] = space;
            return;
        }
        else if (pivot != parts[i].part_start)
        {
            space.start = pivot;
            space.space = parts[i].part_start - pivot;
            space.type = 'f';
            spaces[s] = space;
            pivot = parts[i].part_start;
            s++;
        }
        else
        {
            space.start = parts[i].part_start;
            space.space = parts[i].part_size;
            space.space = parts[i].part_type;
            spaces[s] = space;
            pivot += parts[i].part_size;
            s++;
            i++;
        }
    }
}

MBR sortMBR (MBR mbr)
{
    for (int i = 1; i < 4; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if ((mbr.partitions[j].part_start > mbr.partitions[j + 1].part_start) && mbr.partitions[j + 1].part_start != 0)
            {
                Partition p = mbr.partitions[j];
                mbr.partitions[j] = mbr.partitions[j + 1];
                mbr.partitions[j + 1] = p;
            }
        }
    }
    return mbr;
}

#endif