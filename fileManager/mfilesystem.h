#ifndef M_FILESYSTEM_H
#define M_FILESYSTEM_H

#include "../var/globals.h"
#include "../fileManager/manager.h"

int writeFile_Indirect (char text[], BlockPointer * current, int no_current, int level)
{
    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            BlockPointer * bp = newBlockPointer();
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = sessionVar.superBlock.first_block;
                saveBlock(no_current, current, _BKPOINTER_);
                saveBlock(_EMTPY_, bp, _BKPOINTER_);
                updateBitmap(current->pointers[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                updateSuperBlock();
            }
            else
                bp = (BlockPointer *) getBlock(current->pointers[i], _BKPOINTER_);

            int result = writeFile_Indirect(text, bp, current->pointers[i], level - 1);
            if (result) return result;
        }
        else
        {
            BlockFile * bf = newBlockFile();
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = sessionVar.superBlock.first_block;
                saveBlock(no_current, current, _BKPOINTER_);
                saveBlock(_EMTPY_, bf, _BKFILE_);
                updateBitmap(current->pointers[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                updateSuperBlock();
            }
            else
            {
                bf = (BlockFile *) getBlock(current->pointers[i], _BKFILE_);
                memset(bf->content, 0, 64);
                saveBlock(current->pointers[i], bf, _BKFILE_);
            }

            strcpy(bf->content, text);
            saveBlock(current->pointers[i], bf, _BKFILE_);
            
            return 1;
        }

        return 0;
    }
}

void writeFile (char text[], Inode * current, int no_current, int i)
{
    int level = 1;

    if (i < 12)
    {
        /* BLOQUES DIRECTOS */
        BlockFile * bf = newBlockFile();
        if (current->block[i] == -1)
        {
            current->block[i] = sessionVar.superBlock.first_block;
            saveInode(no_current, current);
            saveBlock(_EMTPY_, bf, _BKFILE_);
            updateBitmap(current->block[i], '1', _BMBLOCK_);
            sessionVar.superBlock.free_blocks_count -= 1;
            sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
            updateSuperBlock();
        }
        else
        {
            bf = (BlockFile *) getBlock(current->block[i], _BKFILE_);
            memset(bf->content, 0, 64);
            saveBlock(current->block[i], bf, _BKFILE_);
        }
        strcpy(bf->content, text);
        saveBlock(current->block[i], bf, _BKFILE_);
    }
    else
    {
        /* BLOQUES INDIRECTOS */
        BlockPointer * bp = newBlockPointer();
        if (current->block[i] == -1)
        {
            current->block[i] = sessionVar.superBlock.first_block;
            saveInode(no_current, current);
            saveBlock(current->block[i], bp, _BKPOINTER_);
            updateBitmap(current->block[i], '1', _BMBLOCK_);
            sessionVar.superBlock.free_blocks_count -= 1;
            sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
            updateSuperBlock();
        }
        else
            bp = (BlockPointer *) getBlock(current->block[i], _BKPOINTER_);
        
        writeFile_Indirect(text, bp, current->block[i], level);
        level++;
    }
}

void contentFromCont (char cont[300], Inode * current, int no_current)
{
    int i = 0;
    int size = 0;
    char text[64] = {0};
    FILE * file = fopen(cont, "r");

    if (file != NULL)
    {
        while(!feof(file) || i < 12){
            fgets(text, 64, file);
            size += strlen(text);
            if (strlen(text) > 0)
                writeFile(text, current, no_current, i);
            memset(text, 0, 64);
            i++;
        }
        
        fclose(file);
    }
}

void contentFromSize (int size, Inode * current, int no_current)
{
    char text[64] = {0};

    int blocks_count = size / 64;
    if (size % 64 > 0)
        blocks_count++;
    
    for (int i = 0; i < 15; i++)
    {
        if (i < blocks_count)
        {
            for (int j = 0, a = 0; j < 64; j++, a++)
            {
                char aux[3] = {j, '\0'};
                strcat(text, aux);
                if (a == 9) a = -1;
            }

            writeFile(text, current, no_current, i);
            memset(text, 0, 64);
        }
        else
            writeFile(text, current, no_current, i);
    }
}

int getInodeByName_Indirect (char name[], BlockPointer * bp, int level)
{
    int no_inode = -1;
    for (int i = 0; i < 16; i++)
    {
        if (bp->pointers[i] < 0) continue;
        if (level > 1){
            no_inode = getInodeByName_Indirect(name, bp, level - 1);
            if (no_inode > 0) return no_inode;
            else continue;
        }
    
        BlockDir * bd = (BlockDir *) getBlock(bp->pointers[i], _BKDIR_);
        for (int j = 0; j < 4; j++)
        {
            if (bd->content[j].inode < 0) continue;
            if (strcmp(name, bd->content[j].name) == 0)
                return bd->content[j].inode;
        }
    }
    return no_inode;
}

int getInodeByName (char name[], Inode * current)
{
    int no_inode = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (current->block[i] < 0) continue;
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            BlockDir * bd = (BlockDir *) getBlock(current->block[i], _BKDIR_);
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0) continue;
                if (strcmp(name, bd->content[j].name) == 0)
                    return bd->content[j].inode;
            }
        }
        else
        {
            /*  BLOQUES INDIRECTOS */
            BlockPointer * bp = (BlockPointer *) getBlock(current->block[i], _BKPOINTER_);
            no_inode = getInodeByName_Indirect(name, bp, level);
            if (no_inode > 0)
                return no_inode;
            level++;
        }
    }

    return no_inode;
}

int createInodeDirectory_Indirect (char name[], BlockPointer * current, int no_upper, int no_current, int level)
{
    int no_inode = -1;

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            BlockPointer * new_bp = newBlockPointer();
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = sessionVar.superBlock.first_block;
                saveBlock(no_current, current, _BKPOINTER_);
                saveBlock(_EMTPY_, new_bp, _BKPOINTER_);
                updateBitmap(current->pointers[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                updateSuperBlock();
            }
            else
                new_bp = (BlockPointer *) getBlock(current->pointers[i], _BKPOINTER_);

            no_inode = createInodeDirectory_Indirect(name, new_bp, no_upper, current->pointers[i], level - 1);
            if (no_inode > 0) return no_inode;
        }
        else
        {
            BlockDir * bd = newBlockDir(_EMTPY_, _EMTPY_);
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = sessionVar.superBlock.first_block;
                saveBlock(no_current, current, _BKPOINTER_);
                saveBlock(_EMTPY_, bd, _BKDIR_);
                updateBitmap(current->pointers[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                updateSuperBlock();
            }
            else
                bd = (BlockDir *) getBlock(current->pointers[i], _BKDIR_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_DIRTYPE_);
                    BlockDir * new_bd = newBlockDir(sessionVar.superBlock.first_inode, no_upper);
                    next->block[0] = sessionVar.superBlock.first_block;
                    bd->content[j].inode = sessionVar.superBlock.first_inode;
                    strcpy(bd->content[j].name, name);
                    saveBlock(current->pointers[i], bd, _BKDIR_);
                    saveInode(_EMTPY_, next);
                    saveBlock(_EMTPY_, new_bd, _BKDIR_);
                    updateBitmap(bd->content[j].inode, '1', _BMINODE_);
                    updateBitmap(next->block[0], '1', _BMBLOCK_);
                    sessionVar.superBlock.free_inodes_count -= 1;
                    sessionVar.superBlock.free_blocks_count -= 1;
                    sessionVar.superBlock.first_inode = getNextFreeBitmap(_BMINODE_);;
                    sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                    updateSuperBlock();
                    return bd->content[j].inode;
                }
            }
        }
    }
    return no_inode;
}

int createInodeDirectory (char name[], Inode * current, int no_current)
{
    int no_next = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            BlockDir * bd = newBlockDir(_EMTPY_, _EMTPY_);
            if (current->block[i] == -1)
            {
                current->block[i] = sessionVar.superBlock.first_block;
                saveInode(no_current, current);
                saveBlock(_EMTPY_, bd, _BKDIR_);
                updateBitmap(current->block[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
            }
            else
                bd = (BlockDir *) getBlock(current->block[i], _BKDIR_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_DIRTYPE_);
                    BlockDir * new_bd = newBlockDir(sessionVar.superBlock.first_block, no_current);
                    next->block[0] = sessionVar.superBlock.first_block;

                    bd->content[j].inode = sessionVar.superBlock.first_inode;
                    strcpy(bd->content[j].name, name);
                    saveBlock(current->block[i], bd, _BKDIR_);
                    saveInode(_EMTPY_, next);
                    saveBlock(next->block[0], &new_bd, _BKDIR_);
                    updateBitmap(bd->content[j].inode, '1', _BMINODE_);
                    updateBitmap(next->block[0], '1', _BMBLOCK_);
                    sessionVar.superBlock.free_inodes_count -= 1;
                    sessionVar.superBlock.free_blocks_count -= 1;
                    sessionVar.superBlock.first_inode = getNextFreeBitmap(_BMINODE_);
                    sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                    updateSuperBlock();
                    return bd->content[j].inode;
                }
            }
        }
        else
        {
            BlockPointer * bp = newBlockPointer();
            if (current->block[i] == -1)
            {
                current->block[i] = sessionVar.superBlock.first_block;
                saveInode(no_current, current);
                saveBlock(_EMTPY_, bp, _BKPOINTER_);
                updateBitmap(current->block[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                updateSuperBlock();
            }
            else
                bp = (BlockPointer *) getBlock(current->block[i], _BKPOINTER_);
            
            no_next = createInodeDirectory_Indirect(name, bp, no_current, current->block[i], level);
            if (no_next > 0)
                return no_next;
            level++;
        }
    }

    return no_next;
}

int createInodeFile_Indirect (char name[], BlockPointer * current, int no_current, int level)
{
    int no_inode = -1;

    for (int i = 0; i < 16; i++)
    {
        if (level > 1)
        {
            BlockPointer * new_bp = newBlockPointer();
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = sessionVar.superBlock.first_block;
                saveBlock(no_current, current, _BKPOINTER_);
                saveBlock(_EMTPY_, new_bp, _BKPOINTER_);
                updateBitmap(current->pointers[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                updateSuperBlock();
            }
            else
                new_bp = (BlockPointer *) getBlock(current->pointers[i], _BKPOINTER_);

            no_inode = createInodeFile_Indirect(name, new_bp, current->pointers[i], level - 1);
            if (no_inode > 0) return no_inode;
        }
        else
        {
            BlockDir * new_bd = newBlockDir(_EMTPY_, _EMTPY_);
            if (current->pointers[i] == -1)
            {
                current->pointers[i] = sessionVar.superBlock.first_block;
                saveBlock(no_current, current, _BKPOINTER_);
                saveBlock(_EMTPY_, new_bd, _BKDIR_);
                updateBitmap(current->pointers[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                updateSuperBlock();
            }
            else
                new_bd = (BlockDir *) getBlock(current->pointers[i], _BKDIR_);
            
            for (int j = 0; j < 4; j++)
            {
                if (new_bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_FILETYPE_);
                    new_bd->content[j].inode = sessionVar.superBlock.first_inode;
                    strcpy(new_bd->content[j].name, name);
                    saveBlock(current->pointers[i], new_bd, _BKDIR_);
                    saveInode(_EMTPY_, next);
                    updateBitmap(new_bd->content[j].inode, '1', _BMINODE_);
                    sessionVar.superBlock.free_inodes_count -= 1;
                    sessionVar.superBlock.first_inode = getNextFreeBitmap(_BMINODE_);;
                    updateSuperBlock();
                    return new_bd->content[j].inode;
                }
            }
        }
    }
    return no_inode;
}

int createInodeFile (char name[], Inode * current, int no_current)
{
    int no_next = -1;
    int level = 1;

    for (int i = 0; i < 15; i++)
    {
        if (i < 12)
        {
            /* BLOQUES DIRECTOS */
            BlockDir * bd = NULL;
            if (current->block[i] == -1)
            {
                bd = newBlockDir(_EMTPY_, _EMTPY_);
                current->block[i] = sessionVar.superBlock.first_block;
                saveInode(no_current, current);
                saveBlock(_EMTPY_, bd, _BKDIR_);
                updateBitmap(current->block[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
            }
            else
                bd = (BlockDir *) getBlock(current->block[i], _BKDIR_);
            
            for (int j = 0; j < 4; j++)
            {
                if (bd->content[j].inode < 0)
                {
                    Inode * next = newInode(_FILETYPE_);
                    bd->content[j].inode = sessionVar.superBlock.first_inode;
                    strcpy(bd->content[j].name, name);
                    saveBlock(current->block[i], bd, _BKDIR_);
                    saveInode(_EMTPY_, next);
                    updateBitmap(bd->content[j].inode, '1', _BMINODE_);
                    sessionVar.superBlock.free_inodes_count -= 1;
                    int next_inode = getNextFreeBitmap(_BMINODE_);
                    sessionVar.superBlock.first_inode = next_inode;
                    updateSuperBlock();
                    return bd->content[j].inode;
                }
            }
        }
        else
        {
            BlockPointer * bp = newBlockPointer();
            if (current->block[i] == -1)
            {
                current->block[i] = sessionVar.superBlock.first_block;
                saveInode(no_current, current);
                saveBlock(current->block[i], bp, _BKPOINTER_);
                updateBitmap(current->block[i], '1', _BMBLOCK_);
                sessionVar.superBlock.free_blocks_count -= 1;
                sessionVar.superBlock.first_block = getNextFreeBitmap(_BMBLOCK_);
                updateSuperBlock();
            }
            else
                bp = (BlockPointer *) getBlock(current->block[i], _BKPOINTER_);
            
            no_next = createInodeFile_Indirect(name, bp, current->block[i], level);
            if (no_next > 0)
                return no_next;
            level++;
        }
    }

    return no_next;
}

#endif