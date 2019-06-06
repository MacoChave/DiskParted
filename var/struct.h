#ifndef STRUCT_H
#define STRUCT_H

typedef struct Partition Partition;
typedef struct MBR MBR;
typedef struct EBR EBR;

typedef struct Mount Mount;
typedef struct PartMount PartMount;

typedef struct Values Values;

struct Partition 
{
    int part_status;
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
    Partition partitions[4];
};

struct EBR 
{
    int part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char ebr_name[16];
};

struct PartMount
{
    char mount_status;
    char mount_type;
    char mount_fit;
    int mount_start;
    int mount_size;
    char mount_id[5];
    char mount_name[16];
};

struct Mount
{
    char path[300];
    int letter;
    PartMount parts_mount[10];
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
};

#endif