/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fs_opendir.c
 *
 * Description: Opens a directory and allows it to be accessible in memory by the rest of the system.
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
fdDir *fs_opendir(const char *pathname)
{
    fdDir *dir = malloc(sizeof(fdDir));
    parsedPath parsed = parsePath(pathname);
    if (parsed.index == -1)
    {
        printf("open error: can't found such directory\n");
        return NULL;
    }
    if (fs_isDir(parsed.parent[parsed.index].fileName) != DIRECTORY)
    {
        printf("open error: this is not a directory\n");
        fs_closedir(dir);
        return NULL;
    }
    dir->lastAccessDate = time(NULL);
    dir->directoryStartLocation = parsed.parent[parsed.index].location;
    dir->dirEntryPosition = 0;
    dir->DE = malloc(parsed.parent[parsed.index].fileSize);
    dir->DE->fileSize = parsed.parent[parsed.index].fileSize;
    strcpy(dir->dirinfo->d_name, parsed.parent[parsed.index].fileName);
    dir->d_reclen = sizeof(fdDir);
    return dir;
}