#ifndef MANAGER_H
#define MANAGER_H

#include <stdio.h>
#include "../var/globals.h"

int createDirectory()
{
    int i = 0;
    char cmd[300] = {0};
    char file_dir[300] = {0};
    char temp[300] = {0};
    strcat(cmd, "mkdir -m 777 -p \"");
    while (values.path[i] != '\0')
    {
        char directory[300] = {values.path[i], '\0'};
        if (values.path[i] != '/' && values.path[i] != '.')
            strcat(temp, directory);
        else if (values.path[i] == '/')
        {
            strcat(file_dir, temp);
            memset(temp, 0, 300);
            strcat(temp, "/");
        }
        else break;

        i++;
    }
    strcat(cmd, file_dir);
    strcat(cmd, "\"");
    return (system(cmd) == 0) ? 1 : 0;
}

int existDisk()
{
    FILE * file;
    file = fopen(values.path, "r");
    
    if (file != NULL)
    {
        fclose(file);
        return 1;
    }
    
    return 0;
}

int createDisk()
{
    FILE * file;
    file = fopen(values.path, "wb");
    
    if (file != NULL)
    {
        char buffer[1024];
        memset(buffer, 0, 1024);

        for (int i = 0; i < values.size / 1024; i++)
        {
            fwrite(buffer, sizeof(buffer), 1, file);
            fflush(file);
        }

        fclose(file);
        return 1;
    }

    return 0;
}

int removeDisk()
{
    return (remove(values.path) == 0) ? 1 : 0;
}

int updateMBR(MBR mbr)
{
    FILE * file;
    file = fopen(values.path, "rb+");
    
    if (file != NULL)
    {
        fwrite(&mbr, sizeof(MBR), 1, file);
        
        fclose(file);
        return 1;
    }

    return 0;
}

MBR getMBR()
{
    MBR mbr;
    mbr.size = 0;
    FILE * file;
    file = fopen(values.path, "rb");

    if (file != NULL)
    {
        fread(&mbr, sizeof(MBR), 1, file);
        fclose(file);
        return mbr;
    }

    return mbr;
}

int updateEBR(EBR ebr, int start)
{
    FILE * file;
    file = fopen(values.path, "rb+");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(&ebr, sizeof(EBR), 1, file);
        fclose(file);
        return 1;
    }

    return 0;
}

EBR getEBR (int start)
{
    EBR ebr;
    ebr.part_next = -1;
    ebr.part_size = -1;
    FILE * file;
    file = fopen(values.path, "rb");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(&ebr, sizeof(EBR), 1, file);
        fclose(file);
        return ebr;
    }

    return ebr;
}

#endif