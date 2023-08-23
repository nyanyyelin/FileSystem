/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fs_isDir.c
 *
 * Description: Checks if directory entry at a path is a directory
 *
 *
 *
 **************************************************************/
#include "mfs.h"
#include "directoryEntry.h"
#include "parsePath.h"

int fs_isDir(char *filename)
{

    parsedPath path = parsePath(filename);
    if (path.index > 0)
    {
        directoryEntry *dir = path.parent;

        if (dir->isFile == DIRECTORY)
        {
            return DIRECTORY;
        }
    }

    return FILEMACRO;
}