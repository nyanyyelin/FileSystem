/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: freeSpaceManager.c
 *
 * Description: Manages free space within filesystem
 *
 *
 *
 **************************************************************/
#include <sys/types.h>
#include "vcb.h"
#include "fsLow.h"
#include "stdlib.h"
#include "freeSpaceManager.h"
#include "stdio.h"
#include "string.h"


/**
 * initFreeSpaceManager initalizes our bitmap array. We reserve space
 * in memory for the manager, then we fill the first blocks that the manager takes up
 * as occupied (1). We then fill up the rest of our bitmap array with 0's to indicate free blocks.
 *
 * We then write to the disk and return the starting position of the freeSpaceManager
 */

int initFreeSpaceManager(int totalBlocks, int blockSize)
{
    // bytes required for our bitmap
    // 1 bit represent 1 block
    // 19531 blocks == 19531 bits == 2442 bytes
    int bytesNeeded = (totalBlocks / 8) + 1;

    // blocks required for our bitmap
    // 5 blocks
    int freeSpaceManagerBlocks = (bytesNeeded / blockSize) + 1;
    vcb->sizeOfFreeSpaceManager = freeSpaceManagerBlocks;

    // initializing our bitmap
    // freeSpaceManager with size 2442 bytes
    unsigned char *freeSpaceManager = (unsigned char *)malloc(bytesNeeded);
    
    // set all the bits to zero
    memset(freeSpaceManager, 0, bytesNeeded);

    // mark first 6 blocks is used.
    // 1 block for VCB and 5 blocks for fsManager
    // total 6 blocks 
    for (int i = 0; i < freeSpaceManagerBlocks + 1; i++)
    {
        setBit(freeSpaceManager, i,1);
    }

    // write back bitMap to disk
    LBAwrite(freeSpaceManager, freeSpaceManagerBlocks, 1);

    // return the starting block of free space
    return 1;
}



void setBit(unsigned char * freeSpaceManager, int bitIndex, int isFree)
{
    // get the byte index;
    int byteIndex = bitIndex / 8;
    // get the bit index in the byte
    int bitIndexInByte = bitIndex % 8;

    // 1 => not free
    // 0 => free
    if(isFree == 1)
    {
        // setting the bit to 1 if isFree == 1
        freeSpaceManager[byteIndex] |= 1 << bitIndexInByte;
    } 
    else
    {
        // setting the bit to zero if isFree == 0
        freeSpaceManager[byteIndex] &=  ~(1 << bitIndexInByte);
    }
}


int checkBit(unsigned char * freeSpaceManager, int bitIndex)
{
    int byteIndex = bitIndex / 8;
    int bitIndexInByte = bitIndex % 8;

    // checking if the bitIndex is zero (free) or not (not free)
    // return 0 if it is bitIndex is free (represent false in c)
    // return non-zero integer if not free (retpresent true in c)
    return (freeSpaceManager[byteIndex] & (1 << bitIndexInByte) ) != 0;
}

/**
 * This function allows the file system to request N amount of blocks,
 * then we check the first group of blocks that are 0 and equals requestedBlocks.
 * Once we find this group, we return the starting index of the blocks.
 */
int findFreeBlocks(int requestedBlocks)
{

    unsigned char * freeSpaceManager = malloc(5 * vcb->blockSize * sizeof(char));
    LBAread(freeSpaceManager,5,1);

    int startIndex = -1;
    int freeBlocks = 0;

    for(int i = 0; i < vcb->totalBlocks - requestedBlocks; i++)
    {
        // if checkBit return false
        // 0(free) reprenset false in c 
        if(!checkBit(freeSpaceManager,i))
        {
            // if first free block
            if(startIndex == -1)
            {
                // set startIndex with first free block
                startIndex = i;
            }
            freeBlocks++;

            if(freeBlocks == requestedBlocks)
            {
                // mark those free contiguous blocks as used
                for(int j = startIndex; j < startIndex + requestedBlocks; j++)
                {
                    setBit(freeSpaceManager,j,1);
                }
                // write back bitMap to disk
                LBAwrite(freeSpaceManager,5,1);
                return startIndex;
            }
        }
        else 
        {
            // resetting if block is not free
            startIndex = -1;
            freeBlocks = 0;
        }

    }
    return -1;
}

void freeBlocks(unsigned char * freeSpaceManager, int startIndex,int numberOfBlocks)
{
    for(int i = startIndex; i < startIndex + numberOfBlocks; i++)
    {
        // setting the bits to zero
        setBit(freeSpaceManager,i,0);
    }
    LBAwrite(freeSpaceManager,5,1);
}


// void printBitMap(unsigned char * freeSpaceManager)
// {
//     for(int i = 0; i < vcb -> totalBlocks; i++)
//     {
//         printf("%d", checkBit(freeSpaceManager,i) ? 1 : 0);
//         if(i % 64 == 63)
//         {
//             printf("\n");
//         }
//     }
//     printf("\n");
// }