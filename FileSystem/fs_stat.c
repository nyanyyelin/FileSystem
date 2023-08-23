/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fs_stat.c
 *
 * Description: Prints information associated with the last child of a path.
 *
 *
 *
 **************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "directoryEntry.h"
#include "mfs.h"
#include "parsePath.h"
#include "vcb.h"
#include "fsLow.h"




int fs_stat(const char * path, struct fs_stat *buf)
{
    parsedPath parsed = parsePath(path);
    
    // getting the index from parsedpath
    int index = parsed.index;

    printf("Index From Parsed Path: %d\n", index);
    // malloc memory to buf to fill in infomation
    buf = malloc(sizeof(fs_stat) * 100);

    // checking if the index is valid
    if (index > -1)
    {
        // setting th values to fs_stat struct
        // and prints out the informations 
        buf->st_size = parsed.parent[index].fileSize;
        printf("Size : %ld\n", buf->st_size);
        
        buf->st_blksize = vcb->blockSize;
        printf("Block Size : %ld\n", buf->st_blksize);
        
        buf->st_blocks = (parsed.parent[index].fileSize / vcb->blockSize) + 1;
        printf("Number of Blocks: %ld\n", buf->st_blocks);
        
        buf->st_accesstime = parsed.parent[index].lastAccessDate;
        printf("Access Time %ld\n", parsed.parent[index].lastAccessDate);
        
        buf->st_modtime = parsed.parent[index].lastModifyDate;
        printf("Modified Time : %ld\n",buf->st_modtime);
        
        buf->st_createtime = parsed.parent[index].createDate;
        printf("Create Time : %ld\n", buf->st_createtime);

        return 0;
        
    }
    // if index is not valid
    return -1;
}