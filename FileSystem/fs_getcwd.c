/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fs_getcwd.c
 *
 * Description: Returns current working directory of specified path
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

char *fs_getcwd(char *pathname, size_t size)
{
    if (strlen(currentWorkDir) > size)
    {//User given buffer is not large enough to store cwd
        return NULL;
    }
    //Return the cwd to user.
    strcpy(pathname, currentWorkDir);
    return currentWorkDir;
}