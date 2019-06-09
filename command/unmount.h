#ifndef UNMOUNT_H
#define UNMOUNT_H

#include "../var/globals.h"
#include "../fileManager/manager.h"
#include "../fileManager/mpartition.h"

void exec_unmount()
{
    if (strlen(values.id) < 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    char id_i = values.id[2];
    int id_a = values.id[3] - '0';

    int i = getDiskById(id_i);
    int j = getPartById(id_a, i);

    if (j == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    disks_mount[i].letter = '0';
    memset(disks_mount[i].path, 0, 300);
    disks_mount[i].parts_mount[j].mount_type = '0';
    disks_mount[i].parts_mount[j].mount_start = 0;
    disks_mount[i].parts_mount[j].mount_size = 0;
    disks_mount[i].parts_mount[j].mount_id = 0;
    memset(disks_mount[i].parts_mount[j].mount_name, 0, 15);

    printf("[i] Partición %s desmontada\n", values.id);
}

#endif