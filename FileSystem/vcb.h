/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: vcb.h
 *
 * Description: Header file that stores our VCB struct, which includes our magic number.
 *
 *
 *
 **************************************************************/

typedef struct
{
    int signature;             // signature to know if filesystem has been initialized
    int blockSize;             // size of each block
    int totalBlocks;           // total blocks available
    int freeBlocks;            // number of free blocks
    int freeSpaceManagerBlock; // starting block for free space manager
    int sizeOfFreeSpaceManager; // size of freeSpace manager
    int rootDirBlock;          // starting block for root directory
    int rootDirSize;           // size of rootDirectory
} VCB;

extern VCB *vcb;