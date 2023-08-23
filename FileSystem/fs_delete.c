/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fs_delete.c
 *
 * Description: Deletes file from disk
 *
 *
 *
 **************************************************************/
#include <stdio.h>
#include "mfs.h"
#include <stdlib.h>
#include <unistd.h>
#include "directoryEntry.h"
#include "fsLow.h"
#include "vcb.h"
#include "parsePath.h"
#include "freeSpaceManager.h"

int fs_delete(char *filename)
{
    parsedPath parsed = parsePath(filename);
    int blkCount = // Calculate filesize in blocks
        (parsed.parent[parsed.index].fileSize + vcb->blockSize - 1) / vcb->blockSize;

    if (parsed.index >= 2 && fs_isFile(parsed.parent[parsed.index].fileName) == FILEMACRO)
    {
        // If path is reachable-> mark entry as avail on disk
        parsed.parent[parsed.index].location = 0;
        parsed.parent[parsed.index].fileSize = 0;
        strcpy(parsed.parent[parsed.index].fileName, "\0");

        LBAwrite(parsed.parent, blkCount, parsed.parent[0].location);
        unsigned char *freeSpaceManager =
            malloc(vcb->sizeOfFreeSpaceManager * vcb->blockSize * sizeof(char));
        freeBlocks(freeSpaceManager,
                   vcb->freeSpaceManagerBlock, vcb->sizeOfFreeSpaceManager);
        free(freeSpaceManager);
        freeSpaceManager = NULL;

        // free memory allocations
        free(parsed.parent);
        free(parsed.path);
        parsed.parent = NULL;
        parsed.path = NULL;
        return 0;
    }

    return -1; // Unsuccessful file delete
}
