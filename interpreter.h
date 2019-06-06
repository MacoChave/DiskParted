#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <ctype.h>
#include "var/globals.h"

#include "command/mkdisk.h"
#include "command/rmdisk.h"

extern void exec_exec();

int loadCommand(char input[]) 
{
    strcat(input, "$"); // EOF

    int i = 0;
    int param = -1;

    int step = 0;
    int quotation_marks = 0;

    char auxiliar[300] = {0};

    if (isMultiline)
    {
        step = _PARAM_;
        isMultiline = !isMultiline;
    }

    while (input[i] != 0)
    {
        if (input[i] == '#')
            return _EXIT_;
        if (input[i] == '\\')
        {
            isMultiline = !isMultiline;
            i++;
            continue;
        }
        if (input[i] == '\n' || input[i] == '\r')
        {
            i++;
            continue;
        }
        if (input[i] == '$')
        {
            if (strlen(auxiliar) > 0)
            {
                switch (param)
                {
                    case _SIZE_:
                        values.size = atoi(auxiliar);
                        break;
                    case _PATH_:
                        strcpy(values.path, auxiliar);
                        break;
                    case _UNIT_:
                        values.unit = auxiliar[0];
                        break;
                    case _NAME_:
                        strcpy(values.name, auxiliar);
                        break;
                    case _TYPE_:
                        values.type = auxiliar[0];
                        break;
                    case _FIT_:
                        values.fit = auxiliar[0];
                        break;
                    case _DELETE_:
                        strcpy(values.del, auxiliar);
                        break;
                    case _ADD_:
                        values.add = atoi(auxiliar);
                        break;
                    case _ID_:
                        strcpy(values.id, auxiliar);
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        if (input[i] == '-' || input[i] == '~')
        {
            i++;
            continue;
        }
        if (isupper(input[i]))
        {
            if (!quotation_marks && param != _PATH_ && param != _NAME_)
                input[i] = tolower(input[i]);
        }

        char current_char[2] = {input[i], '\0'};

        if (step == 0) // OBTENER TIPO COMANDO
        {
            if (input[i] != ' ')
            {
                strcat(auxiliar, current_char);
                i++;
                continue;
            }
            else
            {
                if (strcasecmp(auxiliar, "mkdisk") == 0)
                    command = _MKDISK_;
                else if (strcasecmp(auxiliar, "rmdisk") == 0)
                    command = _RMDISK_;
                else if (strcasecmp(auxiliar, "fdisk") == 0)
                    command = _FDISK_;
                else if (strcasecmp(auxiliar, "mount") == 0)
                    command = _MOUNT_;
                else if (strcasecmp(auxiliar, "unmount") == 0)
                    command = _UNMOUNT_;
                else if (strcasecmp(auxiliar, "rep") == 0)
                    command = _REP_;
                else if (strcasecmp(auxiliar, "exec") == 0)
                    command = _EXEC_;
                else if (strcasecmp(auxiliar, "pause") == 0)
                {
                    char conf[999] = {0};
                    fgets(auxiliar, 999, stdin);
                    return 0;
                }
                else
                {
                    clearValues();
                    printf(ANSI_COLOR_RED "[e] Comando %s no reconocido\n" ANSI_COLOR_RESET, auxiliar);
                    return _ERROR_;
                }
                
                memset(auxiliar, 0, 300);
                step = _PARAM_;
                i++;
                continue;
            }
        }
        else if (step == 1) // OBTENER TIPO PARAMETRO
        {
            if (input[i] != ':')
            {
                strcat(auxiliar, current_char);
                i++;
                continue;
            }
            else
            {
                if (strcasecmp(auxiliar, "size") == 0)
                    param = _SIZE_;
                else if (strcasecmp(auxiliar, "path") == 0)
                    param = _PATH_;
                else if (strcasecmp(auxiliar, "unit") == 0)
                    param = _UNIT_;
                else if (strcasecmp(auxiliar, "name") == 0)
                    param = _NAME_;
                else if (strcasecmp(auxiliar, "type") == 0)
                    param = _TYPE_;
                else if (strcasecmp(auxiliar, "fit") == 0)
                    param = _FIT_;
                else if (strcasecmp(auxiliar, "delete") == 0)
                    param = _DELETE_;
                else if (strcasecmp(auxiliar, "add") == 0)
                    param = _ADD_;
                else if (strcasecmp(auxiliar, "id") == 0)
                    param = _ID_;
                else
                    printf(ANSI_COLOR_RED "[e] Parámetro %s no reconocido\n" ANSI_COLOR_RESET, auxiliar);
                
                memset(auxiliar, 0, 300);
                step = _VALUE_;
                i++;
                continue;
            }
        }
        else if (step == 2) // OBTENER VALOR PARAMETRO
        {
            if (input[i] == '"')
            {
                quotation_marks = !quotation_marks;
                i++;
                continue;
            }
            
            if (input[i] != ' ' && !quotation_marks)
            {
                strcat(auxiliar, current_char);
                i++;
                continue;
            }
            else
            {
                if (quotation_marks)
                {
                    strcat(auxiliar, current_char);
                    i++;
                    continue;
                }
                switch (param)
                {
                    case _SIZE_:
                        values.size = atoi(auxiliar);
                        break;
                    case _PATH_:
                        strcpy(values.path, auxiliar);
                        break;
                    case _UNIT_:
                        values.unit = auxiliar[0];
                        break;
                    case _NAME_:
                        strcpy(values.name, auxiliar);
                        break;
                    case _TYPE_:
                        values.type = auxiliar[0];
                        break;
                    case _FIT_:
                        values.fit = auxiliar[0];
                        break;
                    case _DELETE_:
                        strcpy(values.del, auxiliar);
                        break;
                    case _ADD_:
                        values.add = atoi(auxiliar);
                        break;
                    case _ID_:
                        strcpy(values.id, auxiliar);
                        break;
                    default:
                        break;
                }
                memset(auxiliar, 0, 300);
                step = _PARAM_;
                i++;
                continue;
            }
        }
    }

    if (!isMultiline)
    {
        switch (command)
        {
            case _EXEC_:
                exec_exec();
                break;
            case _MKDISK_:
                exec_mkdisk();
                break;
            case _RMDISK_:
                exec_rmdisk();
                break;
            case _FDISK_:
                break;
            case _MOUNT_:
                break;
            case _UNMOUNT_:
                break;
            case _REP_:
                break;
            default:
                return _EXIT_;
                break;
        }
        clearValues();
    }
}

void exec_exec()
{
    if (strlen(values.path) > 0)
    {
        char input[999] = {0};
        FILE * file;
        file = fopen(values.path, "r");

        if (file != NULL)
        {
            while (!feof(file))
            {
                fgets(input, 999, file);
                sprintf(input, "%s\n", input);
                printf("[d] %s\n", input);
                loadCommand(input);
            }            
        }
        else
            printf(ANSI_COLOR_RED "[e] No se puedo cargar el archivo: %s\n" ANSI_COLOR_RESET, values.path);   
    }
}

#endif