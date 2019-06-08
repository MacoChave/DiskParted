#ifndef REP_H
#define REP_H

#include "../var/globals.h"
#include "../fileManager/mpartition.h"

int getDiskById (char id_i)
{
    for (int i = 0; i < 10; i++)
    {
        if (disks_mount[i].letter == id_i)
            return i;
    }
    return _ERROR_;
}

int getPartById (int id_a, int i)
{
    if (i == _ERROR_) return _ERROR_;

    for (int j = 0; j < 20; j++)
    {
        if (disks_mount[i].parts_mount[j].mount_id == id_a)
            return j;
    }
    return _ERROR_;   
}

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
        if (spaces[i].space > 0)
            count++;
    }
    return count;
}

void reportDisk (MBR mbr, char path_disk[])
{
    char dotfile[15] = "disk_report.dot";
    FILE * file;
    file = fopen(dotfile, "w");

    if (file != NULL)
    {
        
        fclose(file);
    }
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
        fprintf(file, "\trankdir = TD\n");
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
                fprintf(file, "\t\t\t\t<td>part_status%d</td>\n", i);
                fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.partitions[i].part_status);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_type_%d</td>\n", i);
                fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.partitions[i].part_type);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_fit_%d</td>\n", i);
                fprintf(file, "\t\t\t\t<td>%c</td>\n", mbr.partitions[i].part_fit);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_start_%d</td>\n", i);
                fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.partitions[i].part_start);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_size_%d</td>\n", i);
                fprintf(file, "\t\t\t\t<td>%d</td>\n", mbr.partitions[i].part_size);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_name_%d</td>\n", i);
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
                fprintf(file, "\t\t\t\t<td colspan=\"2\">EBR %d Report</td>\n", ebr_index);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>Nombre</td>\n");
                fprintf(file, "\t\t\t\t<td>Valor</td>\n");
                fprintf(file, "\t\t\t</tr>\n");
                
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_status</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", ebr.part_status);
                fprintf(file, "\t\t\t</tr>\n");
                
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_fit</td>\n");
                fprintf(file, "\t\t\t\t<td>%c</td>\n", ebr.part_fit);
                fprintf(file, "\t\t\t</tr>\n");
                
                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_start</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", ebr.part_start);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_size</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", ebr.part_size);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_next</td>\n");
                fprintf(file, "\t\t\t\t<td>%d</td>\n", ebr.part_next);
                fprintf(file, "\t\t\t</tr>\n");

                fprintf(file, "\t\t\t<tr>\n");
                fprintf(file, "\t\t\t\t<td>part_name</td>\n");
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
    // sprintf(cmd, "dot -Tpdf %s -o %s", dotfile, values.path);
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
}

#endif