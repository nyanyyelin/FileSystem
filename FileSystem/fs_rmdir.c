/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fs_rmdir.c
 *
 * Description: Deletes a directory at specified path
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

int fs_rmdir(const char *pathname)
{
     parsedPath parsed = parsePath(pathname); // get parent directory and index of child
    // Calculate size needed to malloc for directory
    int blkCount = (parsed.parent[parsed.index].fileSize + vcb->blockSize - 1) / vcb->blockSize;
    int byteUsed = blkCount * vcb->blockSize;

    if (parsed.index >= 2 && fs_isDir(parsed.parent[parsed.index].fileName) == DIRECTORY)
    { // If path is reachable
        // load dir into memory
        directoryEntry *checkDir = malloc(byteUsed);
        LBAread(checkDir, blkCount, parsed.parent[parsed.index].location);

        // checks the directory for any files/directories before attempting to delete
        int directoryEmpty = directoryIsEmpty(checkDir);
        if (directoryEmpty == 0) // directory was found to be empty-> delete the directory
        {
            parsed.parent[parsed.index].location = 0;           // make location as free
            parsed.parent[parsed.index].fileSize = 0;           // set file size to 0;
            strcpy(parsed.parent[parsed.index].fileName, "\0"); // clear name

            LBAwrite(parsed.parent, blkCount, parsed.parent[0].location);

            unsigned char *freeSpaceManager =
                malloc(vcb->sizeOfFreeSpaceManager * vcb->blockSize * sizeof(char));
            freeBlocks(freeSpaceManager,
                       vcb->freeSpaceManagerBlock, vcb->sizeOfFreeSpaceManager);
            free(freeSpaceManager);
            freeSpaceManager = NULL;
        }
        // free memory allocations
        free(checkDir);
        free(parsed.parent);
        free(parsed.path);

        parsed.parent = NULL;
        parsed.path = NULL;
        checkDir = NULL;
        return 0;
    }

    return -1; // Unsuccessful removal of directory
}