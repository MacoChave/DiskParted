#ifndef REP_H
#define REP_H

#include "../var/globals.h"
#include "../fileManager/mpartition.h"

int getDiskById(char id_i)
{
    for (int i = 0; i < 10; i++)
    {
        if (disks_mount[i].letter == id_i)
            return i;
    }
    return _ERROR_;
}

int getPartById(int id_a, int i)
{
    if (i == _ERROR_) return _ERROR_;

    for (int j = 0; j < 20; j++)
    {
        if (disks_mount[i].parts_mount[j].mount_id == id_a)
            return j;
    }
    return _ERROR_;   
}

void exec_rep ()
{
    if (strlen(values.name) <= 0 || strlen(values.path) <= 0 || strlen(values.id) <= 0)
    {
        printf(ANSI_COLOR_RED "[e] Parámetros incompletos\n" ANSI_COLOR_RESET);
        return;
    }

    clearSpaceDisk();
    char id_i = values.id[2];
    int id_a = values.id[3];

    int i = getDiskById(id_i);
    int j = getPartById(id_a, i);

    if (i == _ERROR_ || j == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }
}

#endif