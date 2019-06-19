#ifndef MKFS_H
#define MKFS_H

#include <time.h>
#include <math.h>
#include "../var/globals.h"
#include "../fileManager/manager.h"
#include "../fileManager/mpartition.h"
#include "../fileManager/mfilesystem.h"

#define __USER__ "1,G,root\n1,U,root,root,123\n"

void settingFileSystem ()
{
    /* CREAR INODO ROOT Y PRIMER BLOQUE */
    Inode * root = newInode(_DIRTYPE_);
    BlockDir* bd = newBlockDir(0, 0);
    root->block[0] = 0;
    saveInode(0, root);
    saveBlock(0, bd, _BKDIR_);
    updateBitmap(0, '1', _BMINODE_);
    updateBitmap(0, '1', _BMBLOCK_);
    sessionVar.superBlock.free_blocks_count -= 1;
    sessionVar.superBlock.free_inodes_count -= 1;
    int next_inode = getNextFreeBitmap(_BMINODE_);
    int next_block = getNextFreeBitmap(_BMBLOCK_);
    sessionVar.superBlock.first_inode = next_inode;
    sessionVar.superBlock.first_block = next_block;
    updateSuperBlock();

    /* CREAR INODO USER.TXT Y ESCRIBIR GRUPO Y USUARIO ROOT */

    int no_newin = createInodeFile("users.txt", root, 0);
    Inode * next = getInode(no_newin);
    writeFile(__USER__, next, no_newin, 0);
}

void settingBitmaps ()
{
    for (int i = 0; i < sessionVar.superBlock.inodes_count; i++)
        updateBitmap(i, '0', _BMINODE_);
    for (int i = 0; i < sessionVar.superBlock.blocks_count; i++)
        updateBitmap(i, '0', _BMBLOCK_);
}

void settingJounal (int part_start)
{
    int initWorkspace = part_start + sizeof(SuperBlock);
    Journal journal = clearJournal();

    for (int i = 0; i < sessionVar.superBlock.inodes_count; i++)
    {
        updateJounal(journal, initWorkspace);
        initWorkspace += sizeof(Journal);
    }    
}

void settingSuperBlock (Partition part)
{
    SuperBlock sb;
    int size_sb = sizeof(SuperBlock);
    int size_j = sizeof(Journal);
    int size_in = sizeof(Inode);
    int size_bq = sizeof(BlockDir);
    int startWorkspace = part.part_start;
    
    sb.fylesystem_type = values.fs - '0';
    sb.magic = 61267;
    sb.mnt_count = 1;
    __time_t currentDate = time(NULL);
    struct tm * date = localtime(&currentDate);
    strftime(sb.mtime, sizeof(sb.mtime) - 1, "%d/%m/%y %H:%M", date);
    strftime(sb.umtime, sizeof(sb.umtime) - 1, "%d/%m/%y %H:%M", date);

    /* Calcular n */
    double n = 0;
    double num_structs = 0;
    double dividend = part.part_size - size_sb;
    double divider = size_j + size_in + 3 * size_bq + 4;
    num_structs = dividend / divider;
    n = floor(num_structs);

    sb.inodes_count = n;
    sb.blocks_count = 3 * n;
    sb.free_inodes_count = n;
    sb.free_blocks_count = 3 * n;

    startWorkspace += size_sb + n * size_j;

    sb.bm_inode_start = startWorkspace;
    sb.bm_block_start = startWorkspace + n;
    sb.inode_start = startWorkspace + n + 3 * n;
    sb.block_start = startWorkspace + n + 3 * n + n * size_in;

    sb.first_inode = 0;
    sb.first_block = 0;
    sb.inode_size = size_in;
    sb.block_size = size_bq;

    sessionVar.superBlock = sb;
}

void exec_mkfs ()
{
    if (strlen(values.id) == 0)
    {
        printf(ANSI_COLOR_RED "[e] Se requiere id de partición\n" ANSI_COLOR_RESET);
        return;
    }

    char id_x = values.id[2];
    int id_y = values.id[3] - '0';

    int i = getDiskById(id_x);
    int j = getPartById(id_y, i);

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
            if (strcmp(mbr.partitions[a].part_name, disks_mount[a].parts_mount[a].mount_name) == 0)
            {
                part = mbr.partitions[a];
                break;
            }
        }
    }

    if (i == 4) return;
    
    if (values.type != 'n')
        clearPartDisk(disks_mount[i].path, part.part_start, part.part_start + part.part_size);
    
    sessionVar.part_start = part.part_start;
    strcpy(sessionVar.path, disks_mount[i].path);
    settingSuperBlock(part);
    updateSuperBlock();
    settingJounal(part.part_start);
    settingBitmaps();
    settingFileSystem();
}

#endif