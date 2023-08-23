/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: parsePath.h
 *
 * Description: Path parser and utility procedures header
 *
 * 
 *
 **************************************************************/
#include "directoryEntry.h"
typedef struct
{
    int index; //index at which child exists
    char dirName[64];  //directory name
    char childName[64];//name of child
    char  * path; //absolute path
    directoryEntry * parent;  //pointer to location of directory
}parsedPath;

void loadDirectory(directoryEntry * loadDir, int entryIndex);//loads directory into given buffer
int locateEntry( directoryEntry * dir, char * entry);//searches directory for an entry
parsedPath parsePath(const char * path); //parses a path and returns parsePath object
int findOpenEntrySlot(directoryEntry * parent);//searches directory for an avail slot
int directoryIsEmpty(directoryEntry * parent);//checks whether directory is empty or not
char * resolvePath(const char *path);//creates an absolute path