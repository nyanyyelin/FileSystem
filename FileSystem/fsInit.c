/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: fsInit.c
 *
 * Description: Main driver for file system assignment.
 *
 * This file is where you will start and initialize your system
 *
 **************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "fsLow.h"
#include "mfs.h"
#include "vcb.h"
#include "directoryEntry.h"
#include "freeSpaceManager.h"
#define magicNumber 734743927 // random signature

VCB *vcb; // global declaration of VCB

int initRootDir(int blockSize)
{
	// 52 directory entries * 136 sizeof 1 directory entry = 7072 bytes/ 512 chunks = 14 blocks

	directoryEntry *dir = (directoryEntry *)malloc(initDirAmt);
	dir = initDir(initDirAmt, NULL);
	int rootposition = dir[0].location;
	vcb->rootDirSize = dir->fileSize;

	free(dir);
	dir = NULL;

	return rootposition;
}

int initFileSystem(uint64_t numberOfBlocks, uint64_t blockSize)
{
	printf("Initializing File System with %ld blocks with a block size of %ld\n", numberOfBlocks, blockSize);
	/* TODO: Add any code you need to initialize your file system. */
	vcb = malloc(blockSize);

	if (LBAread(vcb, 1, 0) == 1) // Read block 0 from disk into VCB
	{

		if (vcb->signature != magicNumber) // System not initalized
		{
			// init vcb since signature is missing or does not match
			vcb->signature = magicNumber;
			vcb->blockSize = blockSize;
			vcb->totalBlocks = numberOfBlocks;
			vcb->freeBlocks = numberOfBlocks;

			// struct fs_stat *test;
			// int result = fs_stat("~",test);
			// printf("RESULT : %d\n",result);

			// Set vals returned from init procedures
			vcb->freeSpaceManagerBlock = initFreeSpaceManager(vcb->totalBlocks, vcb->blockSize);
			vcb->rootDirBlock = initRootDir(vcb->blockSize);

			LBAwrite(vcb, 1, 0); // LBAwrite VCB back to disk
		}
		else
		{
			// loadFreeSpace into memory
			printf("Welcome back!\n");
		}
		return 0;
	}

	return -1; // Unable to read from disk
}	

void exitFileSystem()
{
	printf("System exiting\n");
	free(vcb);
	vcb = NULL;
}