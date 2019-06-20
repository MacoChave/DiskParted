#ifndef LOGIN_H
#define LOGIN_H

#include "../var/globals.h"
#include "../fileManager/mpartition.h"
#include "../fileManager/mfilesystem.h"

typedef struct Row
{
    char text[40];
} Row;

Row rows[20];

void initRowsAux ()
{
    for (int i = 0; i < 20; i++)
        memset(rows[i].text, 0, 40);
}

int loadUserTable()
{
    Inode * root = getInode(0);
    int no_inode = getInodeByName("users.txt", root);
    return no_inode;
}

void fillUsersTable (int no_inode)
{
    Inode * current = getInode(no_inode);
    if (current == NULL) return;
    char * text = NULL;
    initRowsAux();
    text = readFile(current);
    
    int i = 0;
    char * str_row;
    str_row = strtok(text, "\n");
    
    while (str_row != NULL)
    {
        strcpy(rows[i].text, str_row);
        str_row = strtok(NULL, "\n");
        i++;
    }

    i = 0;
    int no_col = 0;
    char * str_columns;

    while (i < 20)
    {
        str_columns = strtok(rows[i].text, ",");
        while (str_columns != NULL)
        {
            switch (no_col)
            {
            case 0: // id
                permissions[i].state = atoi(str_columns);
                break;
            case 1: // type
                permissions[i].type = str_columns[0];
                break;
            case 2: // group
                strcpy(permissions[i].group, str_columns);
                break;
            case 3: // user
                strcpy(permissions[i].name, str_columns);
                break;
            case 4: //pass
                strcpy(permissions[i].pass, str_columns);
                break;
            default:
                break;
            }
            no_col++;
            str_columns = strtok(NULL, ",");
        }
        no_col = 0;
        i++;
    }
}

int validateUser ()
{
    for (int i = 0; i < 20; i++)
    {
        if (permissions[i].state > 0 && permissions[i].type == 'U')
        {
            if (strcmp(permissions[i].name, values.usr) == 0 && strcmp(permissions[i].pass, values.pwd) == 0)
                return i;
        }
    }
    return -1;
}

void exec_login ()
{
    if (sessionVar.id_user >= 0)
    {
        printf(ANSI_COLOR_RED "[e] Cierre sesión antes de iniciar una nueva sesión\n" ANSI_COLOR_RESET);
        return;
    }
    if (strlen(values.usr) < 0 || strlen(values.pwd) < 0 || strlen(values.id) < 0 )
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos para iniciar sesión\n" ANSI_COLOR_RESET);
        return;
    }

    char id_i = values.id[2];
    int id_j = values.id[3] - '0';

    int i = getDiskById(id_i);
    int j = getPartById(id_j, i);

    if (j == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    MBR mbr = getMBR(disks_mount[i].path);
    Partition part;
    int a;
    for (a = 0; a < 4; a++)
    {
        if (mbr.partitions[a].part_type == 'p')
        {
            if (strcmp(mbr.partitions[a].part_name, disks_mount[i].parts_mount[j].mount_name) == 0)
            {
                part = mbr.partitions[a];
                break;
            }
        }
    }

    if (a == 4) return;
    strcpy(sessionVar.path, disks_mount[i].path);
    sessionVar.part_start = part.part_start;
    sessionVar.superBlock = getSuperBlock();
    int no_inode = loadUserTable();
    if (no_inode < 0)
    {
        printf(ANSI_COLOR_RED "[e] No se encontró users.txt\n" ANSI_COLOR_RESET);
        return;
    }
    
    fillUsersTable (no_inode);
    int id_user = validateUser();
    if (id_user < 0)
    {
        printf(ANSI_COLOR_RED "[e] Usuario y/o contraseña incorrecta\n" ANSI_COLOR_RESET);
        return;
    }
    sessionVar.id_user = id_user;
}

#endif