/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fs_setcwd.c
 *
 * Description: Sets current working directory to specified path
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

int fs_setcwd(char *pathname)
{
    parsedPath parsed;
    parsed = parsePath(pathname); //Checks for valid path
    if (parsed.index > -1)
    {
        strcpy(currentWorkDir, parsed.path); //Update cwd

        // free memory allocations
        free(parsed.parent);
        free(parsed.path); 
        parsed.parent = NULL;
        parsed.path = NULL;
        
        return 0;
    }
    return -1; //Unsuccessful in setting cwd
}