/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: b_io.c
 *
 * Description: Buffering I/O handling for the user
 *
 *
 *
 **************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "fsLow.h"
#include "b_io.h"
#include "vcb.h"
#include "parsePath.h"
#include "freeSpaceManager.h"
#include "directoryEntry.h"
#define MAXFCBS 20
#define B_CHUNK_SIZE 512
#define FILE_BLOCKS 50

typedef struct b_fcb
{
	/** TODO add al the information you need in the file control block **/
	char *buf;		// holds the open file buffer
	int index;		// holds the current position in the buffer
	int buflen;		// holds how many valid bytes are in the buffer
	int currentBlk; // holds the current bloc number
	int numBlocks;	// holds how many blocks file occupies
	int fileOffset; // holds the current position that the file is at
	int flags;
	directoryEntry * parent; // for write back
	int dirindex;
	directoryEntry * DE;
} b_fcb;

b_fcb fcbArray[MAXFCBS];

int startup = 0; // Indicates that this has not been initialized

// Method to initialize our file system
void b_init()
{
	// init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
	{
		fcbArray[i].buf = NULL; // indicates a free fcbArray
	}

	startup = 1;
}

// Method to get a free FCB element
b_io_fd b_getFCB()
{
	for (int i = 0; i < MAXFCBS; i++)
	{
		if (fcbArray[i].buf == NULL)
		{
			return i; // Not thread safe (But do not worry about it for this assignment)
		}
	}
	return (-1); // all in use
}

// Interface to open a buffered file
// Modification of interface for this assignment, flags match the Linux flags for open
// O_RDONLY, O_WRONLY, or O_RDWR
b_io_fd b_open(char *filename, int flags)
{
	b_io_fd returnFd;
	//*** TODO ***:  Modify to save or set any information needed
	//
	//

	if (startup == 0)
		b_init(); // Initialize our system

	returnFd = b_getFCB(); // get our own file descriptor
						   // check for error - all used FCB's
	if (returnFd < 0)
	{
		return -1;
	}
	parsedPath parsed = parsePath(filename);
	int pos=0;
	//if file exists
	if(parsed.index==-1){
		if(flags & O_CREAT){
			time_t t= time(NULL);
			int start = findOpenEntrySlot(parsed.parent);
			strcpy(parsed.parent[start].fileName,parsed.childName);
			parsed.parent[start].createDate=t;
			parsed.parent[start].lastAccessDate=t;
			parsed.parent[start].lastModifyDate=t;
			parsed.parent[start].location = findFreeBlocks(FILE_BLOCKS);
			parsed.parent[start].fileSize = 0;
			parsed.parent[start].isFile = FILEMACRO;
			memcpy(fcbArray[returnFd].DE, &parsed.parent[start],sizeof(directoryEntry));
		}
	}else{
		if(flags & O_TRUNC){
			parsed.parent[parsed.index].fileSize = 0;
		}
		if(flags & O_APPEND){
			//move file position to end
			pos=(parsed.parent[parsed.index].fileSize-1);
		}
		memcpy(fcbArray[returnFd].DE, &parsed.parent[parsed.index],sizeof(directoryEntry));
	}
	fcbArray[returnFd].parent = malloc(parsed.parent->fileSize);
	memcpy(fcbArray[returnFd].parent, &parsed.parent,parsed.parent->fileSize);
	fcbArray[returnFd].dirindex = parsed.index;
	fcbArray[returnFd].flags = flags;
	fcbArray[returnFd].buf = malloc(B_CHUNK_SIZE);
	fcbArray[returnFd].buflen = B_CHUNK_SIZE;
	fcbArray[returnFd].index = 0;
	fcbArray[returnFd].currentBlk = 0;
	fcbArray[returnFd].numBlocks = (parsed.parent[parsed.index].fileSize + B_CHUNK_SIZE - 1) / B_CHUNK_SIZE;
	fcbArray[returnFd].fileOffset = pos;
	return (returnFd); // all set
}

// Interface to seek function
int b_seek(b_io_fd fd, off_t offset, int whence)
{
	if (startup == 0)
		b_init(); // Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
	{
		return (-1); // invalid file descriptor
	}
	if (whence == SEEK_SET)
	{
		//repositions the file offset to the offset passed by the caller.
		fcbArray[fd].fileOffset = offset;
	}
	else if (whence == SEEK_CUR)
	{
		//repositions the file offset to the current location plus caller offset bytes
		fcbArray[fd].fileOffset += offset;
	}
	else if (whence == SEEK_END) 
	{
		// repositions the file offset to the size of the file plus caller offset bytes
		fcbArray[fd].fileOffset = (fcbArray[fd].numBlocks * B_CHUNK_SIZE) + offset;
	}

	return fcbArray[fd].fileOffset; // Change this
}

// Interface to write function
int b_write(b_io_fd fd, char *buffer, int count)
{
	int part1, part2, part3;
	int leftover, blocks;
	if (startup == 0)
		b_init(); // Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
	{
		return (-1); // invalid file descriptor
	}
	if(!((fcbArray[fd].flags&O_WRONLY)||(fcbArray[fd].flags&O_RDWR))){
		printf("This file cannot write\n");
		return -1;
	}
	fcbArray[fd].currentBlk = fcbArray[fd].DE->fileSize/B_CHUNK_SIZE;
	fcbArray[fd].index = fcbArray[fd].DE->fileSize%B_CHUNK_SIZE;
	int maxbytes = FILE_BLOCKS*B_CHUNK_SIZE;
	if(count+fcbArray[fd].DE->fileSize >maxbytes){
		count = maxbytes - fcbArray[fd].DE->fileSize;
		printf("the file is not big enough\n");
	}
	int remain = fcbArray[fd].buflen - fcbArray[fd].index;
	if(count>remain){
		part1 = remain;
		leftover = count - part1;
		blocks = leftover/B_CHUNK_SIZE;
		part2 = blocks*B_CHUNK_SIZE;
		part3 = leftover - part2;
	}else{
		part1 = count;
		fcbArray[fd].index+=part1;
		part2 = 0;
		part3 = 0;
	}
	memcpy(fcbArray[fd].buf+fcbArray[fd].index,buffer,part1);
	if(part1>remain){
		LBAwrite(buffer,1,fcbArray[fd].DE->location+fcbArray[fd].currentBlk);
		fcbArray[fd].currentBlk++;
		fcbArray[fd].buf = NULL;
		fcbArray[fd].index=0;
	}
	// if part2>0 it's larger than a blocksize
	if(part2>0){
		
		LBAwrite(buffer+part1,blocks,fcbArray[fd].DE->location+fcbArray[fd].currentBlk);
		fcbArray[fd].currentBlk+=blocks;
		
	}
	if(part3>0){
		memcpy(fcbArray[fd].buf+fcbArray[fd].index,buffer+part1+part2,part3);
	}
	int bytesWrite = part1+part2+part3;
	fcbArray[fd].DE->fileSize+=bytesWrite;
	fcbArray[fd].DE->lastAccessDate = time(NULL);
	fcbArray[fd].DE->lastModifyDate = time(NULL);

	return count; // Change this
}

// Interface to read a buffer

// Filling the callers request is broken into three parts
// Part 1 is what can be filled from the current buffer, which may or may not be enough
// Part 2 is after using what was left in our buffer there is still 1 or more block
//        size chunks needed to fill the callers request.  This represents the number of
//        bytes in multiples of the blocksize.
// Part 3 is a value less than blocksize which is what remains to copy to the callers buffer
//        after fulfilling part 1 and part 2.  This would always be filled from a refill
//        of our buffer.
//  +-------------+------------------------------------------------+--------+
//  |             |                                                |        |
//  | filled from |  filled direct in multiples of the block size  | filled |
//  | existing    |                                                | from   |
//  | buffer      |                                                |refilled|
//  |             |                                                | buffer |
//  |             |                                                |        |
//  | Part1       |  Part 2                                        | Part3  |
//  +-------------+------------------------------------------------+--------+
int b_read(b_io_fd fd, char *buffer, int count)
{
	int part1, part2, part3;
	int leftover, blocks;
	if (startup == 0)
		b_init(); // Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
	{
		return (-1); // invalid file descriptor
	}
	if(!((fcbArray[fd].flags&O_RDONLY)||(fcbArray[fd].flags&O_RDWR))){
		printf("This file cannot read\n");
		return -1;
	}
	if(fcbArray[fd].DE == NULL){
		printf("This file have not opened yet\n");
		return -1;
	}
	if(count+fcbArray[fd].fileOffset>fcbArray[fd].DE->fileSize){
		count = fcbArray[fd].DE->fileSize - fcbArray[fd].fileOffset;
	}
	int remain = fcbArray[fd].buflen - fcbArray[fd].index;
	if(count>remain){
		part1 = remain;
		leftover = count - part1;
		blocks = leftover/B_CHUNK_SIZE;
		part2 = blocks*B_CHUNK_SIZE;
		part3 = leftover - part2;
	}else{
		part1 = count;
		fcbArray[fd].index+=part1;
		part2 = 0;
		part3 = 0;
	}
	LBAread(fcbArray[fd].buf,1,fcbArray[fd].DE->location+fcbArray[fd].currentBlk);
	fcbArray[fd].currentBlk++;
	if(part1>0){
		memcpy(buffer,fcbArray[fd].buf+fcbArray[fd].index,part1);
	}
	// if part2>0 it's larger than a blocksize
	if(part2>0){
		
		LBAread(buffer+part1,blocks,fcbArray[fd].DE->location+fcbArray[fd].currentBlk);
		fcbArray[fd].currentBlk+=blocks;
		
	}
	if(part3>0){
		//reset index and buffer
		fcbArray[fd].buf = NULL;
		fcbArray[fd].index=0;
		LBAread(fcbArray[fd].buf,1,fcbArray[fd].DE->location+fcbArray[fd].currentBlk);
		memcpy(buffer+part1+part2,fcbArray[fd].buf+fcbArray[fd].index,part3);
	}
	int bytesRead = part1+part2+part3;
	fcbArray[fd].fileOffset+=bytesRead;
	fcbArray[fd].DE->lastAccessDate = time(NULL);
	return count; // Change this
}

// Interface to Close the file
int b_close(b_io_fd fd)
{
	if(fcbArray[fd].DE==NULL){
		printf("the file have not open yet\n");
		return -1;
	}
	if((fcbArray[fd].flags&O_RDONLY)||(fcbArray[fd].flags&O_RDWR)){
		LBAwrite(fcbArray[fd].buf,1,fcbArray[fd].DE->location+fcbArray[fd].currentBlk);
	}
	memcpy(&fcbArray[fd].parent[fcbArray[fd].dirindex],fcbArray[fd].DE,sizeof(directoryEntry));
	LBAwrite(fcbArray[fd].parent,(fcbArray[fd].parent->fileSize/vcb->blockSize)+1,fcbArray[fd].parent->location);
	free(fcbArray[fd].parent);
	fcbArray[fd].parent = NULL;
	fcbArray[fd].dirindex = 0;
	fcbArray[fd].DE = NULL;
	fcbArray[fd].buflen = 0;
    fcbArray[fd].flags = 0;
    fcbArray[fd].numBlocks = 0;
	fcbArray[fd].currentBlk = 0;
	fcbArray[fd].fileOffset = 0;
	fcbArray[fd].index = 0;
    free(fcbArray[fd].buf);
    fcbArray[fd].buf = NULL;
}
