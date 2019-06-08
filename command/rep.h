#ifndef REP_H
#define REP_H

#include "../var/globals.h"
#include "../fileManager/mpartition.h"

int countPrimary (Partition part[])
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (part[i].part_size > 0)
            count++;
    }
    return count;
}

int countLogical ()
{
    int count = 0;
    for (int i = 0; i < 10; i++)
    {
        if (spaces[i].type == 'l')
            count+= 2;
        if (spaces[i].type == 'f')
            count++;
    }
    return count;
}

void reportDisk (MBR mbr, char path_disk[])
{
    char dotfile[15] = "disk_report.dot";
    FILE * file;
    file = fopen(dotfile, "w");
    int no_p = countPrimary(mbr.partitions);
    int no_l = countLogical();
    double size = 0;
    int pivot = sizeof(MBR);

    if (file != NULL)
    {
        fprintf(file, "digraph {\n");
        fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
        fprintf(file, "\tnode [shape = plain]\n");
        fprintf(file, "\trankdir = LR\n");
        fprintf(file, "\tDISK [label=<\n");
        fprintf(file, "\t\t<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
        size = (sizeof(MBR) * 100) / mbr.size;
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td rowspan=\"2\">MBR <br/> %.4f %%</td>\n", size);
        
        for (int i = 0; i < 4; i++)
        {
            if (pivot != mbr.partitions[i].part_start)
            {
                if (mbr.partitions[i].part_size > 0)
                {
                    size = (mbr.partitions[i].part_start - pivot * 100) / mbr.size;
                    pivot = mbr.partitions[i].part_start;
                }
                else
                {
                    size = (mbr.size - pivot * 100) / mbr.size;
                    pivot = mbr.size;
                    i = 4;
                }
                fprintf(file, "\t\t\t\t<td rowspan=\"2\">Free <br/> %.2f %%</td>\n", size);
            }
            if (mbr.partitions[i].part_type == 'p')
            {
                pivot = mbr.partitions[i].part_start + mbr.partitions[i].part_size;
                size = (mbr.partitions[i].part_size * 100) / mbr.size;
                fprintf(file, "\t\t\t\t<td rowspan=\"2\">%s <br/> %.2f %%</td>\n", mbr.partitions[i].part_name, size);
            }
            else if (mbr.partitions[i].part_type == 'e')
            {
                pivot = mbr.partitions[i].part_start + mbr.partitions[i].part_size;
                fprintf(file, "\t\t\t\t<td colspan=\"%d\">Extendida</td>\n", no_l);
            }
        }
        if (pivot < mbr.size)
        {
            size = (mbr.size - pivot * 100) / mbr.size;
            fprintf(file, "\t\t\t\t<td rowspan=\"2\">Free <br/> %.2f %%</td>\n", size);
        }
        fprintf(file, "\t\t\t</tr>\n");

        int idx_ext = getNumberExtendedPart(mbr.partitions);
        if (idx_ext != _ERROR_)
        {
            fprintf(file, "\t\t\t<tr>\n");
            for (int i = 0; i < 10; i++)
            {
                if (spaces[i].type == 'l')
                {
                    EBR ebr = getEBR(path_disk, spaces[i].start);
                    fprintf(file, "\t\t\t\t<td>EBR</td>\n");
                    size = (spaces[i].space * 100) / mbr.size;
                    fprintf(file, "\t\t\t\t<td>%s <br/> %.2f %%</td>\n", ebr.ebr_name, size);
                }
                else if (spaces[i].type == 'f')
                {
                    size = (spaces[i].space * 100) / mbr.size;
                    fprintf(file, "\t\t\t\t<td>Free <br/> %.2f %%</td>\n", size);
                }
            }
            fprintf(file, "\t\t\t</tr>\n");
        }
        fprintf(file, "\t\t</table>\n");
        fprintf(file, "\t>]\n");
        fprintf(file, "}\n");

        fclose(file);
    }

    char cmd[300] = {0};
    strcpy(cmd, "dot -Tjpg ");
    strcat(cmd, "disk_report.dot");
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    system(cmd);
}

void reportMBR (MBR mbr, char path_disk[])
{
    char dotfile[15] = "mbr_report.dot";
    FILE * file;
    file = fopen(dotfile, "w");

    if (file != NULL)
    {
        fprintf(file, "digraph {\n");
        fprintf(file, "\tgraph[pad=\"0.5\", nodesep=\"0.5\", ranksep=\"2\"]\n");
        fprintf(file, "\tnode [shape = plain]\n");
        fprintf(file, "\trankdir = LR\n");
        fprintf(file, "\tMBR [label=<\n");
        fprintf(file, "\t\t<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
        
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td colspan=\"2\">MBR Report</td>\n");
        fprintf(file, "\t\t\t</tr>\n");

        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td>Nombre</td>\n");
        fprintf(file, "\t\t\t\t<td>Valor</td>\n");
        fprintf(file, "\t\t\t</tr>\n");
        
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td>MBR Tamaño</td>\n");
        fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.size);
        fprintf(file, "\t\t\t</tr>\n");
        
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td>MBR Fecha de Creación</td>\n");
        fprintf(file, "\t\t\t\t<td>%s</td>\n", mbr.mbr_creation);
        fprintf(file, "\t\t\t</tr>\n");
        
        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td>MBR Disk Signature</td>\n");
        fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.mbr_disk_signature);
        fprintf(file, "\t\t\t</tr>\n");

        fprintf(file, "\t\t\t<tr>\n");
        fprintf(file, "\t\t\t\t<td>Disk fit</td>\n");
        fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.fit);
        fprintf(file, "\t\t\t</tr>\n");
        
        for (int i = 0; i < 4; i++)
        {
            if (mbr.partitions[i].part_size > 0)
            {
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td colspan=\"2\">Partición %d</td>\n", i);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part status</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.partitions[i].part_status);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part type</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.partitions[i].part_type);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part fit</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.partitions[i].part_fit);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part start</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.partitions[i].part_start);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part size</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.partitions[i].part_size);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part name</td>\n");
                fprintf(file, "\t\t\t\t<td>%s</td>\n", mbr.partitions[i].part_name);
                fprintf(file, "\t\t\t</tr>\n");
            }
        }

        fprintf(file, "\t\t</table>\n");
        fprintf(file, "\t>]\n");


        int ebr_index = 1;
        EBR ebr;
        for (int i = 0; i < 50; i++)
        {
            if (spaces[i].type == 'l')
            {
                ebr = getEBR(path_disk, spaces[i].start);
                fprintf(file, "\tEBR%d [label=<\n", ebr_index);
                fprintf(file, "\t\t<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
            
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td colspan=\"2\">Reporte EBR %d</td>\n", ebr_index);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Nombre</td>\n");
                fprintf(file, "\t\t\t\t<td>Valor</td>\n");
                fprintf(file, "\t\t\t</tr>\n");
                
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part status</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", ebr.part_status);
                fprintf(file, "\t\t\t</tr>\n");
                
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part fit</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", ebr.part_fit);
                fprintf(file, "\t\t\t</tr>\n");
                
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part start</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", ebr.part_start);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part size</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", ebr.part_size);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part next</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", ebr.part_next);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Part name</td>\n");
                fprintf(file, "\t\t\t\t<td>%s</td>\n", ebr.ebr_name);
                fprintf(file, "\t\t\t</tr>\n");
            
                fprintf(file, "\t\t</table>\n");
                fprintf(file, "\t>]\n");
                ebr_index++;
            }
        }
        
        fprintf(file, "}");
        fclose(file);
    }

    char cmd[300] = {0};
    strcpy(cmd, "dot -Tpdf ");
    strcat(cmd, dotfile);
    strcat(cmd, " -o ");
    strcat(cmd, values.path);
    system(cmd);
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
    int id_a = values.id[3] - '0';

    int i = getDiskById(id_i);
    int j = getPartById(id_a, i);

    if (i == _ERROR_ || j == _ERROR_)
    {
        printf(ANSI_COLOR_RED "[e] La partición %s no se encuentra montada\n" ANSI_COLOR_RESET, values.id);
        return;
    }

    if(!createDirectory(values.path))
    {
        printf(ANSI_COLOR_RED "[e] No se pudo crear el directorio %s\n" ANSI_COLOR_RESET, values.path);
        return;
    }
    
    MBR mbr = getMBR(disks_mount[i].path);
    int i_ext = getNumberExtendedPart(mbr.partitions);
    EBR ebr = getEBR(disks_mount[i].path, mbr.partitions[i_ext].part_start);
    getSpaceLogicalDetail(disks_mount[i].path, ebr, mbr.partitions[i_ext].part_start + mbr.partitions[i_ext].part_size);

    if (strcmp(values.name, "disk") == 0)
        reportDisk(mbr, disks_mount[i].path);
    else if (strcmp(values.name, "mbr") == 0)
        reportMBR(mbr, disks_mount[i].path);
    
    clearSpaceDisk();
}

#endif