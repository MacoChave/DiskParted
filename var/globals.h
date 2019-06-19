#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "struct.h"

/* TIPO DE COMANDO */
#define _EXIT_ -1
#define _EXEC_ 1
#define _MKDISK_ 2
#define _RMDISK_ 3
#define _FDISK_ 4
#define _MOUNT_ 5
#define _UNMOUNT_ 6
#define _REP_ 7
#define _PAUSE_ 8
#define _MKFS_ 10
#define _LOGIN_ 11
#define _LOGOUT_ 12
#define _MKGRP_ 13
#define _RMGRP_ 14
#define _MKUSR_ 15
#define _RMUSR_ 16
#define _CHMOD_ 17
#define _MKFILE_ 18
#define _CAT_ 17
#define _REM_ 18
#define _EDIT_ 19
#define _REN_ 20
#define _MKDIR_ 21
#define _CP_ 22
#define _MV_ 23
#define _FIND_ 24
#define _CHOWN_ 25
#define _CHGRP_ 26
#define _RECOVERY_ 27
#define _LOSS_ 28

/* TIPO DE PARAMETRO */
#define _SIZE_ 1
#define _PATH_ 2
#define _UNIT_ 3
#define _NAME_ 4
#define _TYPE_ 5
#define _FIT_ 6
#define _DELETE_ 7
#define _ADD_ 8
#define _ID_ 9
#define _FS_ 10
#define _USR_ 11
#define _PWD_ 12
#define _GRP_ 13
#define _UGO_ 14
#define _CONT_ 15
#define _FILE_ 16
#define _DEST_ 17
#define _RUTA_ 18

#define _COMMAND_ 0
#define _PARAM_ 1
#define _VALUE_ 2
#define _ERROR_ -2
#define _EMTPY_ -3
#define _FILETYPE_ '1'
#define _DIRTYPE_ '0'
#define _BKDIR_ 'b'
#define _BKFILE_ 'f'
#define _BKPOINTER_ 'p'
#define _BMBLOCK_ 'b'
#define _BMINODE_ 'i'

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

Mount disks_mount[10];
Permission permissions[20];
Values values;
SessionVar sessionVar;

int isMultiline;
int command;

void initDisksMount ()
{
    for (int i = 0; i < 10; i++)
    {
        disks_mount[i].letter = '0';
        memset(&disks_mount[i].path, 0, 300);
        for (int j = 0; j < 20; j++)
        {
            disks_mount[i].parts_mount[j].mount_type = '0';
            disks_mount[i].parts_mount[j].mount_start = 0;
            disks_mount[i].parts_mount[j].mount_size = 0;
            disks_mount[i].parts_mount[j].mount_id = 0;
            
            memset(disks_mount[i].parts_mount[j].mount_name, 0, 16);
        }
    }
}

void initPermissionList ()
{
    for (int i = 0; i < 20; i++)
    {
        permissions[i].state = 0;
        permissions[i].type = '0';
        memset(permissions[i].group, 0, 10);
        memset(permissions[i].name, 0, 10);
        memset(permissions[i].pass, 0, 10);
    }    
}

Journal clearJournal ()
{
    Journal journal;
    journal.operation = _ERROR_;
    journal.type_inode = _ERROR_;
    memset(journal.content, 0, 300);
    memset(journal.date, 0, 16);
    memset(journal.name, 0, 300);
    memset(journal.owner, 0, 10);
    memset(journal.permission, 0, 3);

    return journal;
}

Inode clearInode (char type)
{
    Permission perm = permissions[sessionVar.id_user];
    Inode inode;
    inode.uid = perm.state;
    inode.gid = perm.type - '0';
    inode.permission = 664;
    inode.size = 0;
    inode.type = type;
    
    memset(inode.atime, 0, 16);
    memset(inode.ctime, 0, 16);
    memset(inode.mtime, 0, 16);
    __time_t currentDate = time(NULL);
    struct tm * date = localtime(&currentDate);
    strftime(inode.atime, sizeof(inode.atime) - 1, "%d/%m/%y %H:%M", date);
    strftime(inode.ctime, sizeof(inode.ctime) - 1, "%d/%m/%y %H:%M", date);
    strftime(inode.mtime, sizeof(inode.mtime) - 1, "%d/%m/%y %H:%M", date);
    for (int i = 0; i < 15; i++)
        inode.block[i] = -1;
    
    return inode;
}

Inode * newInode (char type)
{
    Permission perm = permissions[sessionVar.id_user];
    Inode * inode = (Inode *) calloc(1, sizeof(Inode));
    inode->uid = perm.state;
    inode->gid = perm.type - '0';
    inode->permission = 664;
    inode->size = 0;
    inode->type = type;
    
    memset(inode->atime, 0, 16);
    memset(inode->ctime, 0, 16);
    memset(inode->mtime, 0, 16);
    __time_t currentDate = time(NULL);
    struct tm * date = localtime(&currentDate);
    strftime(inode->atime, sizeof(inode->atime) - 1, "%d/%m/%y %H:%M", date);
    strftime(inode->ctime, sizeof(inode->ctime) - 1, "%d/%m/%y %H:%M", date);
    strftime(inode->mtime, sizeof(inode->mtime) - 1, "%d/%m/%y %H:%M", date);
    for (int i = 0; i < 15; i++)
        inode->block[i] = -1;
    
    return inode;
}

BlockDir clearBlockDir (int current, int upper)
{
    BlockDir bd;
    for (int i = 0; i < 4; i++)
    {
        if (i == 0)
        {
            if (current != _EMTPY_)
            {
                bd.content[i].inode = current;
                strcpy(bd.content[i].name, ".");
            }
        }
        else if (i == 1)
        {
            if (upper != _EMTPY_)
            {
                bd.content[i].inode = upper;
                strcpy(bd.content[i].name, "..");
            }
        }
        else
        {
            bd.content[i].inode = -1;
            memset(bd.content[i].name, 0, 12);
        }
        
    }
    
    return bd;
}

BlockDir * newBlockDir (int current, int upper)
{
    BlockDir * bd = (BlockDir *)calloc(1, sizeof(BlockDir));
    for (int i = 0; i < 4; i++)
    {
        if (i == 0 && current != _EMTPY_)
        {
            bd->content[i].inode = current;
            strcpy(bd->content[i].name, ".");
        }
        else if (i == 1 && upper != _EMTPY_)
        {
            bd->content[i].inode = upper;
            strcpy(bd->content[i].name, "..");
        }
        else
            bd->content[i].inode = -1;
    }

    return bd;
}

BlockFile clearBlockFile ()
{
    BlockFile bf;
    memset(bf.content, 0, 64);

    return bf;
}

BlockFile * newBlockFile ()
{
    BlockFile * bf = (BlockFile *) calloc(1, sizeof(BlockFile));

    return bf;
}

BlockPointer clearBlockPointer ()
{
    BlockPointer bp;
    for (int i = 0; i < 16; i++)
        bp.pointers[i] = -1;
    
    return bp;
}

BlockPointer * newBlockPointer ()
{
    BlockPointer *bp = (BlockPointer *) calloc(1, sizeof(BlockPointer));
    for (int i = 0; i < 16; i++)
        bp->pointers[i] = -1;
    
    return bp;
}

void initSessionVar ()
{
    sessionVar.id_user = 0;
    sessionVar.part_start = 0;
    memset(sessionVar.path, 0, 300);
}

void clearValues ()
{
    memset(values.path, 0, 300);
    memset(values.del, 0, 300);
    memset(values.name, 0, 300);
    memset(values.id, 0, 300);
    values.fit = '0';
    values.unit = '0';
    values.type = '0';
    values.size = 0;
    values.add = 0;

    values.isRecursive = 0;
    values.fs = '2';
    memset(values.usr, 0, 10);
    memset(values.pwd, 0, 10);
    memset(values.grp, 0, 10);
    memset(values.ugo, 1, 3);
    memset(values.cont, 0, 300);
    memset(values.dest, 0, 300);
    memset(values.ruta, 0, 300);
    
    isMultiline = 0;
    command = -1;
}

void clearDiskMounted (int i)
{
    for (int j = 0; j < 20; j++)
    {
        if (disks_mount[i].parts_mount[j].mount_id != 0)
            return;
    }
    disks_mount[i].letter = '0';
    memset(disks_mount[i].path, 0, 300);
}


void clearPartMounted (int i, int j)
{
    disks_mount[i].parts_mount[j].mount_type = '0';
    disks_mount[i].parts_mount[j].mount_start = 0;
    disks_mount[i].parts_mount[j].mount_size = 0;
    disks_mount[i].parts_mount[j].mount_id = 0;
    memset(disks_mount[i].parts_mount[j].mount_name, 0, 16);   
}

void clearAllPartMounted (int i)
{
    for (int j = 0; j < 30; j++)
        clearPartMounted(i, j);    
}
#endif