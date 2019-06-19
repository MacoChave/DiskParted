#ifndef MANAGER_H
#define MANAGER_H

#include <stdio.h>
#include "../var/globals.h"

int createDirectory(char path[])
{
    int i = 0;
    char cmd[300] = {0};
    char file_dir[300] = {0};
    char temp[300] = {0};
    strcat(cmd, "mkdir -m 777 -p -v \"");
    while (path[i] != '\0')
    {
        char directory[300] = {path[i], '\0'};
        if (path[i] != '/' && path[i] != '.')
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

/* 
 * /home/archivos/fase2/reportes/tree1.png -> /home/archivos/fase2/reportes
 */
char * getDirectory(const char *filename)
{
    const char *s = filename;
    char *fileDir = (char *)calloc(strlen(filename), sizeof(char));
    char *temp = (char *)calloc(strlen(filename), sizeof(char));
    
    while (*s)
    {
        if (*s != '/' && *s != '.')
            sprintf(temp, "%s%c", temp, *s);
        else if (*s == '/')
        {
            strcat(fileDir, temp);
            memset(temp, 0, strlen(filename));
            strcpy(temp, "/");
        }
        else
            break;
        *s++;
    }
    free(temp);
    temp = NULL;

    return fileDir;
}

/* 
 * /home/archivos/fase2/reportes/tree1.png -> tree1.png
 */
char * getFileNamePath(const char *path)
{
    const char * s = path;

    char * name = (char *)malloc(strlen(path));
    memset(name, 0, strlen(path));
    while(*s)
    {
        if (*s != '/')
            sprintf(name, "%s%c", name, *s);
        else if (*s == '/')
            memset(name, 0, strlen(path));
        *s++;
    }

    return name;
}

/* 
 * /home/archivos/fase2/reportes/tree1.png -> tree1
 */
char * getNamePath(const char *path)
{
    const char * s = path;

    char * name = (char *)malloc(strlen(path));
    memset(name, 0, strlen(path));
    while(*s)
    {
        if (*s != '/' && *s != '.')
            sprintf(name, "%s%c", name, *s);
        else if (*s == '/')
            memset(name, 0, strlen(path));
        else if (*s == '.') break;
        *s++;
    }

    return name;
}

/* 
 * /home/archivos/fase2/reportes/tree1.png -> png
 */
char * getExtensionPath (char * filename)
{
    char * s = filename;

    char * extension = (char *)malloc(strlen(filename));
    memset(extension, 0, strlen(filename));

    while(*s)
    {
        if (*s != '/' && *s != '.')
            sprintf(extension, "%s%c", extension, *s);
        else if (*s == '/' || *s == '.')
            memset(extension, 0, strlen(filename));
        
        *s++;
    }

    return extension;
}

void createReport (char * directory, char * name, char * ext)
{
    char cmd[125];
    memset(cmd, 0, 125);
    strcpy(cmd, "dot -T");
    strcat(cmd, ext);
    strcat(cmd, " '");
    strcat(cmd, directory);
    strcat(cmd, "/");
    strcat(cmd, name);
    strcat(cmd, ".dot' -o '");
    strcat(cmd, directory);
    strcat(cmd, "/");
    strcat(cmd, name);
    strcat(cmd, ".");
    strcat(cmd, ext);
    strcat(cmd, "'");
    printf(ANSI_COLOR_BLUE "[d] %s\n" ANSI_COLOR_RESET, cmd);
}

int existDisk(char path[])
{
    FILE * file;
    file = fopen(path, "r");
    
    if (file != NULL)
    {
        fclose(file);
        return 1;
    }
    
    return 0;
}

int createDisk(char path[])
{
    FILE * file;
    file = fopen(path, "wb");
    
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

int clearPartDisk(char path[], int start, int size)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        char buffer[1024];
        memset(buffer, 0, 1024);

        for (int i = start; i < size / 1024; i++)
        {
            fwrite(buffer, sizeof(buffer), 1, file);
            fflush(file);
        }
        fclose(file);
        return 1;
    }

    return 0;
}

int removeDisk(char path[])
{
    return (remove(path) == 0) ? 1 : 0;
}

int updateMBR(char path[], MBR mbr)
{
    FILE * file;
    file = fopen(path, "rb+");
    
    if (file != NULL)
    {
        fwrite(&mbr, sizeof(MBR), 1, file);
        
        fclose(file);
        return 1;
    }

    return 0;
}

MBR getMBR(char path[])
{
    MBR mbr;
    mbr.size = 0;
    FILE * file;
    file = fopen(path, "rb");

    if (file != NULL)
    {
        fread(&mbr, sizeof(MBR), 1, file);
        fclose(file);
        return mbr;
    }

    return mbr;
}

int updateEBR(char path[], EBR ebr, int start)
{
    FILE * file;
    file = fopen(path, "rb+");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(&ebr, sizeof(EBR), 1, file);
        fclose(file);
        return 1;
    }

    return 0;
}

EBR getEBR (char path[], int start)
{
    EBR ebr;
    ebr.part_next = -1;
    ebr.part_size = -1;
    FILE * file;
    file = fopen(path, "rb");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(&ebr, sizeof(EBR), 1, file);
        fclose(file);
        return ebr;
    }

    return ebr;
}

int updateSuperBlock ()
{
    int start = sessionVar.part_start;
    FILE * file;
    file = fopen(sessionVar.path, "rb+");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(&sessionVar.superBlock, sizeof(SuperBlock), 1, file);
        fclose(file);
        return 1;
    }

    return 0;
}

SuperBlock getSuperBlock ()
{
    int start = sessionVar.part_start;
    SuperBlock sb;
    sb.blocks_count = 0;
    sb.inodes_count = 0;
    
    FILE * file;
    file = fopen(sessionVar.path, "rb");

    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(&sb, sizeof(SuperBlock), 1, file);
        fclose(file);
        return sb;
    }

    return sb;
}

int updateJounal(Journal journal, int start)
{
    FILE * file;
    file = fopen(sessionVar.path, "rb+");
    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(&journal, sizeof(Journal), 1, file);
        fclose(file);
        return 1;
    }
    return 0;
}

int updateBitmap (int n, char bit, char type)
{
    int start = (type == _BMBLOCK_) ? 
        sessionVar.superBlock.bm_block_start + n : 
        sessionVar.superBlock.bm_inode_start + n;
    
    FILE * file;
    file = fopen(sessionVar.path, "rb+");
    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(&bit, sizeof(char), 1, file);
        fclose(file);
        return 1;
    }
    return 0;
}

char getBitmap (int n, char type)
{
    int start = (type == _BMBLOCK_) ? 
        sessionVar.superBlock.bm_block_start + n : 
        sessionVar.superBlock.bm_inode_start + n;
    char bit = '0';

    FILE * file;
    file = fopen(sessionVar.path, "rb");
    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(&bit, sizeof(char), 1, file);
        fclose(file);
    }
    return bit;
}

int getNextFreeBitmap (char type)
{
    char bit = '-';
    int i = 0;
    int start = 0;
    int end = 0;
    FILE * file;
    file = fopen(sessionVar.path, "rb");
    if (file != NULL)
    {
        if (type == _BMBLOCK_)
        {
            start = sessionVar.superBlock.bm_block_start;
            end = sessionVar.superBlock.blocks_count;
        }
        else
        {
            start = sessionVar.superBlock.bm_inode_start;
            end = sessionVar.superBlock.inodes_count;
        }
        fseek(file, start, SEEK_SET);
        while (i < end)
        {
            fread(&bit, 1, 1, file);
            if (bit == '0')
                break;
            i++;
        }
        fclose(file);
        return i;
    }
    return i;
}

int saveBlock (int n, void * block, char type)
{
    n = (n == _EMTPY_) ? sessionVar.superBlock.first_block : n;
    int start = sessionVar.superBlock.block_start + n * sizeof(BlockDir);
    FILE * file;
    file = fopen(sessionVar.path, "rb+");
    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        if (type == _BKDIR_)
            fwrite(block, sizeof(BlockDir), 1, file);
        else if (type == _BKFILE_)
            fwrite(block, sizeof(BlockFile), 1, file);
        else if (type == _BKPOINTER_)
            fwrite(block, sizeof(BlockPointer), 1, file);
        fclose(file);
        return 1;
    }
    return 0;
}

/**
 * @brief Obtiene un bloque [directorio | archivo | puntero] de la partición actual
 * @param n: Número de bloque
 * @param type: Tipo de bloque a buscar [d=directorio | f=archivo | p=puntero]
 */
void * getBlock (int n, char type)
{
    int start = sessionVar.superBlock.block_start + n * sizeof(BlockDir);
    void * result = NULL;
    FILE * file;
    file = fopen(sessionVar.path, "rb");
    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        if (type == _BKDIR_)
        {
            result = (BlockDir *)calloc(1, sizeof(BlockDir));
            fread(result, sizeof(BlockDir), 1, file);
        }
        else if (type == _BKFILE_)
        {
            result = (BlockFile *)calloc(1, sizeof(BlockFile));
            fread(result, sizeof(BlockFile), 1, file);
        }
        else if (type == _BKPOINTER_)
        {
            result = (BlockPointer *)calloc(1, sizeof(BlockPointer));
            fread(result, sizeof(BlockPointer), 1, file);
        }
        fclose(file);
    }
    return result;
}

int saveInode (int n, Inode * current)
{
    n = (n == _EMTPY_) ? sessionVar.superBlock.first_inode : n;
    int start = sessionVar.superBlock.inode_start + n * sizeof(Inode);
    FILE * file;
    file = fopen(sessionVar.path, "rb+");
    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fwrite(current, sizeof(Inode), 1, file);
        fclose(file);
        return 1;
    }
    return 0;
}

Inode * getInode (int n)
{
    int start = sessionVar.superBlock.inode_start + n * sizeof(Inode);
    Inode * in = newInode(_DIRTYPE_);
    in->type = -1;
    FILE * file;
    file = fopen(sessionVar.path, "rb");
    if (file != NULL)
    {
        fseek(file, start, SEEK_SET);
        fread(in, sizeof(Inode), 1, file);
        fclose(file);
    }
    return in;
}

#endif