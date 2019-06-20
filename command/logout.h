#ifndef LOGOUT_H
#define LOGOUT_H

#include "../var/globals.h"

void exec_logout ()
{
    if (sessionVar.id_user < 0)
    {
        printf("[e] No hay usuario logueado\n");
        return;
    }

    sessionVar.part_start = 0;
    memset(sessionVar.path, 0, 300);
    sessionVar.id_user = -1;
}

#endif