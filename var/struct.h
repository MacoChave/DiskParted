#ifndef STRUCT_H
#define STRUCT_H

typedef struct Partition Partition;
typedef struct MBR MBR;
typedef struct EBR EBR;

typedef struct Mount Mount;
typedef struct PartMount PartMount;

typedef struct SuperBlock SuperBlock;
typedef struct Journal Journal;
typedef struct Inode Inode;
typedef struct Content Content;
typedef struct BlockDir BlockDir;
typedef struct BlockFile BlockFile;
typedef struct BlockPointer BlockPointer;

typedef struct Values Values;
typedef struct SpaceDisk SpaceDisk;
typedef struct Permission Permission;
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

struct SuperBlock
{
    int fylesystem_type;
    int inodes_count;
    int blocks_count;
    int free_blocks_count;
    int free_inodes_count;
    char mtime[16];
    char umtime[16];
    int magic; // 0xEF53
    int mnt_count;
    int inode_size;
    int block_size;
    int first_inode;
    int first_block;
    int bm_inode_start;
    int bm_block_start;
    int inode_start;
    int block_start;
};

struct Journal
{
    char operation;
    char type_inode;
    char name[300];
    char content[300];
    char date[16];
    char owner[10];
    char permission[3];
};

struct Inode
{
    int uid;
    int gid;
    int size;
    char atime[16];
    char ctime[16];
    char mtime[16];
    int block[15];
    char type;
    int permission;
};

struct Content
{
    char name[12];
    int inode;
};

struct BlockDir
{
    Content content[4];
};

struct BlockFile
{
    char content[64];
};

struct BlockPointer
{
    int pointers[16];
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

struct Permission
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
    int part_start;
    char path[300];
    SuperBlock superBlock;
};

#endif