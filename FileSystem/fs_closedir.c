/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fs_closedir.c
 *
 * Description: Adds functionality to close a directory and all of it's associated resources. 
 * This will free up a file descriptor in our file control block array. 
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
int fs_closedir(fdDir *dirp)
{
    if (dirp == NULL)
    {
        printf("close error: no such directory\n");
        return -1;
    }
    dirp->lastAccessDate = time(NULL);
    dirp->dirEntryPosition = 0;
    dirp->dirinfo = NULL;
    dirp->directoryStartLocation = 0;
    free(dirp->DE);
    free(dirp->dirinfo);
    free(dirp);
    return 0;
}
