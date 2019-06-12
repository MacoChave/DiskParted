#ifndef STRUCT_H
#define STRUCT_H

typedef struct Partition Partition;
typedef struct MBR MBR;
typedef struct EBR EBR;

typedef struct Mount Mount;
typedef struct PartMount PartMount;

typedef struct Values Values;
typedef struct SpaceDisk SpaceDisk;
typedef struct PermissionList PermissionList;
typedef struct SessionVar SessionVar;

struct Partition 
{
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
};

struct MBR 
{
    int size;
    char mbr_creation[16];
    int mbr_disk_signature;
    char fit;
    Partition partitions[4];
};

struct EBR 
{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char ebr_name[16];
};

struct PartMount
{
    char mount_type;
    int mount_start;
    int mount_size;
    int mount_id;
    char mount_name[16];
};

struct Mount
{
    char path[300];
    char letter;
    PartMount parts_mount[30];
};

struct Values
{
    char path[300];
    char del[300];
    char name[300];
    char id[300];
    char fit;
    char unit;
    char type;
    int size;
    int add;

    int isRecursive;
    char fs;
    char usr[10];
    char pwd[10];
    char grp[10];
    char ugo[3];
    char cont[300];
    char dest[300];
    char ruta[300];
    // VARIABLE PARA FILE_N[300][20]
};

struct SpaceDisk
{
    int start;
    int space;
    char type;
    int next;
    int prev;
};

struct PermissionList
{
    int state;
    char type;
    char group[10];
    char name[10];
    char pass[10];
};

struct SessionVar
{
    int id_user;
    char path[300];
};


#endif