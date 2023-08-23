/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fs_mkdir.c
 *
 * Description: Creates a directory at specified path
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

int fs_mkdir(const char *pathname, mode_t mode)
{
    parsedPath parsed = parsePath(pathname); // get parent directory and index of child
    if (parsed.parent != NULL && parsed.index == -1)
    // if a path was valid and child does not exist
    {

        // Searches parent directory for available slot
        int entryIndex = findOpenEntrySlot(parsed.parent);
        printf("entryIndex: %d\n", entryIndex);
        if (entryIndex >= 2) // empty index to create directory was found
        {
            // Create directory
            directoryEntry *createDir = initDir(initDirAmt, parsed.parent);
            // copy directory info into parent
            // set filename
            strcpy(parsed.parent[entryIndex].fileName, parsed.dirName);
            //set filesize of child directory
            parsed.parent[entryIndex].fileSize = createDir->fileSize;
            // set start location in parent directory
            parsed.parent[entryIndex].location = createDir->location;
            // update metadata such as lastModified date
            parsed.parent[entryIndex].lastModifyDate = createDir->lastModifyDate;
            // mark entry as a directory
            parsed.parent[entryIndex].isFile = DIRECTORY;

            // writes back to disk
            int blkCount = 
                (parsed.parent[entryIndex].fileSize + vcb->blockSize - 1) / vcb->blockSize;
            LBAwrite(parsed.parent, blkCount, parsed.parent->location);

            // free memory allocations
            free(parsed.parent);
            free(parsed.path);
            parsed.parent = NULL;
            parsed.path = NULL;
            return 0;
        }
    }
    // Unable to reach directory
    printf("Failed to make directory.\n");
    return -1;
}