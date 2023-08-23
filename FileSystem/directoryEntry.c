/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: directoryEntry.c
 *
 * Description: Creates a directory within a given directory
 *
 *
 *
 **************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "vcb.h"
#include "fsLow.h"
#include "directoryEntry.h"
#include "freeSpaceManager.h"
#include "mfs.h"

char currentWorkDir[]; //global var to store cwd

directoryEntry *initDir(int minNumEntries, directoryEntry *parent)
//Creates a directory within a given directory

{
	int bytesNeed = minNumEntries * sizeof(directoryEntry);
	int blkCount = (bytesNeed + vcb->blockSize - 1) / vcb->blockSize;
	int byteUsed = blkCount * vcb->blockSize;
	int trueNumEntries = byteUsed / sizeof(directoryEntry);
	bytesNeed = trueNumEntries * sizeof(directoryEntry);

	directoryEntry *dir = malloc(byteUsed);

	int startBlock = findFreeBlocks(blkCount); //Requests a starting block from freespace

	for (int i = 2; i < trueNumEntries; i++) //sets the directories as available for use
	{
		strcpy(dir[i].fileName, "\0");
	}

	time_t t = time(NULL); //new time instance
	//create "." folder and initalize all its metadata
	strcpy(dir[0].fileName, "."); 
	dir[0].createDate = t;
	dir[0].lastModifyDate = t;
	dir[0].lastAccessDate = t;
	dir[0].location = startBlock;
	dir[0].fileSize = bytesNeed;
	dir[0].isFile = DIRECTORY;
	strcpy(dir[1].fileName, "..");
	if (parent == NULL)
	{
		//if parent == NULL, the root directory will be created as ".."(parent)
		//initialize metadata
		dir[1].createDate = dir[0].createDate;
		dir[1].lastModifyDate = dir[0].lastModifyDate;
		dir[1].lastAccessDate = dir[0].lastAccessDate;
		dir[1].location = dir[0].location;
		dir[1].fileSize = dir[0].fileSize;
		dir[1].isFile = dir[0].isFile;
	}
	else
	{
		//else give the parent's metadata and info to ".."
		dir[1].createDate = parent->createDate;
		dir[1].lastModifyDate = parent->lastModifyDate;
		dir[1].lastAccessDate = parent->lastAccessDate;
		dir[1].location = parent->location;
		dir[1].fileSize = parent->fileSize;
		dir[1].isFile = parent->isFile;
	}
	LBAwrite(dir, blkCount, startBlock); //write back to disk
	return dir;
}