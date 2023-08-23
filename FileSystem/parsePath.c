/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: parsePath.c
 *
 * Description: Path parser and utility functions
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

void loadDirectory(directoryEntry *loadDir, int entryIndex) // Loads Directory into memory
{
    int dirStartBlock = loadDir[entryIndex].location;
    int blkCount = (loadDir[entryIndex].fileSize + vcb->blockSize - 1) / vcb->blockSize;
    LBAread(loadDir, blkCount, dirStartBlock);
}

int locateEntry(directoryEntry *dir, char *entry)
// Searches parent directory for matching entry
{ // search directory, entry = name
    for (int i = 0; i < (dir->fileSize) / sizeof(directoryEntry); i++)
    {
        // Checks whether the current index i matches the index being searched for
        if (strcmp(dir[i].fileName, entry) == 0)
        {
            return i; // If a match, return index;
        }
    }
    // If no match is found, -1 is returned.
    return -1;
}

parsedPath parsePath(const char *path) // Parses a path and returns a struct containing
// a pointer to the parent directory, the index of entry, and the absolute path
{
    char pathname[256]; // Pointer to tokenize
    // Calculate bytes needed for dir malloc
    int blkCount = (vcb->rootDirSize+ vcb->blockSize - 1) / vcb->blockSize;
    int byteUsed = blkCount * vcb->blockSize;

    directoryEntry *dirToParse = malloc(byteUsed); // to read and access directories
    parsedPath pathRet; // return struct
    pathRet.path = resolvePath(path); // else, resolve path to return
    strcpy(pathname, pathRet.path);  // copy path for tokenizing
    
    if (path == NULL)
    {
        // There is nothing to tokenize
        // return nulls and 0s
        pathRet.index = 0;
        pathRet.parent = NULL;
        strcpy(pathRet.path, "\0");
        strcpy(pathRet.dirName, "\0");
        return pathRet;
    }
    else if (strcmp(pathname, "~") == 0 || // return root Directory for cd ~ command
    strcmp(pathname, "/") == 0 ) //if path resolve function == "/"
    {
        LBAread(dirToParse, blkCount, vcb->rootDirBlock); // ptr to root directory
        pathRet.index = 0;                                // no child index specified
        pathRet.parent = dirToParse;                      // parent of root is root
        return pathRet;
    }
    
    LBAread(dirToParse, blkCount, vcb->rootDirBlock); // loads root directory
    char *token;                                      // for tokenizing path
    char *endingToken = NULL;                         // to get the next pointer
    char *rest;                                       // rest pointer
    token = strtok_r(pathname, "/", &rest);
    if (token != NULL)
    {
        endingToken = strtok_r(NULL, "/", &rest); // grab .next ptr
    }
    int entryIndex = -1; // sets index to not found yet
    while (endingToken != NULL)
    {
        entryIndex = locateEntry(dirToParse, token);
        token = endingToken;
        endingToken = strtok_r(NULL, "/", &rest);

        if (entryIndex == -1 &&
            (dirToParse[entryIndex].isFile == FILEMACRO && endingToken != NULL))
        {
            // Cannot continue since entry is a file and more to process
            pathRet.index = entryIndex;
            pathRet.parent = NULL;
            return pathRet;
        }
        // loads directory at found index
        loadDirectory(dirToParse, entryIndex);
    }
    pathRet.index = locateEntry(dirToParse, token); // returns the index of the child directory
    pathRet.parent = dirToParse;
    strcpy(pathRet.dirName, token);
    if (endingToken != NULL)
    {
        strcpy(pathRet.childName, endingToken);
    }
    return pathRet;
}

int findOpenEntrySlot(directoryEntry *parent) // Searches parent dir for open dir entry
{
    for (int i = 0; i < (parent->fileSize) / sizeof(directoryEntry); i++)
    {

        // Searches for an empty directory slot
        if (strcmp(parent[i].fileName, "\0") == 0)
        {
            // returns the index of the empty slot if directory isn't full
            return i;
        }
    }
    // If directory is full, -1 will be returned
    return -1;
}

int directoryIsEmpty(directoryEntry *parent)
// Checks if parent directory has any sub entries
{
    // If a directory has a filename that is not null, then it is considered as not empty
    for (int i = 2; i < (parent->fileSize) / sizeof(directoryEntry); i++)
    {
        if (strcmp(parent[i].fileName, "\0") != 0)
        {
            // a non null filename was found, therefore directory has files/directories
            return -1;
        }
    }
    // all files are null, therefore no files/directories found
    return 0;
}

char *resolvePath(const char *path)
{
    char resolve[256];
    char *resolvedPath = malloc(256); // return pointer
    strcpy(resolve,"\0"); //clear resolve
    if(strcmp(path, "~") == 0){
        strcpy(resolvedPath,"/");
        return resolvedPath;
    }
    if (path[0] != '/') // If path does not being with "/", it is relative
    {                   // Since the path is relative, we must add the cwd prefix
        strcpy(resolve, currentWorkDir);
        strcat(resolve,"/");
    }
    // concat the rest of the path the create an absolute path
    strcat(resolve, path);


    char *token;// for tokenizing the absolute path
    char *rest1;// rest pointer for tokenizing absolute path
    token = strtok_r(resolve, "/", &rest1); // splits token using "/" as a delimiter
    char *tempArr[50];// An array to store each token temporarily
    int i = 0; // index i to have direct access to current token in the array

    for (int i = 0; i < 50; i++)
    { // Sets array to NULL
        tempArr[i] = NULL;
    }

    while (token != NULL) // while there are more tokens left to read
    {
        if (strcmp(token, "..") == 0)
        { // If current token == "..", undo the previous token add and decrement i
            // Go back
            if (i > 0)
            {
                i--;
                tempArr[i] = NULL;
            }
        }
        else if (strcmp(token, ".") != 0) // this is to ignore "." and add any other token
        {
            // Add if != '.'
            tempArr[i] = token; // adds token to array
            i++;                // increments
        }
        token = strtok_r(NULL, "/", &rest1); // grabs next token to be parsed
    }
    
    i = 0; // resets i so that the tokens that haven't been
    // removed can be added to the return pointer
    
    strcpy(resolvedPath, "/");  // adds rootDir prefix
    while (tempArr[i] != NULL)
    {
        // Adds each token from the array into the resolved path
        // and follows each with file separator
        strcat(resolvedPath, tempArr[i]);
        strcat(resolvedPath, "/");
        i++;
    }
    if(strlen(resolvedPath )>1){
        resolvedPath[strlen(resolvedPath)-1] = '\0';
    }
    
    // free(resolve);       // free resolve
    // resolve = NULL;      // sets resolve to NULL
    return resolvedPath; // the resolved path
}