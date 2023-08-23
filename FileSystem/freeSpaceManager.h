/**************************************************************
 * Class:  CSC-415-01 Spring 2023
 * Names: Anthony Benjamin, Nyan Ye Lin, Joshua Hayes, David Chen
 * Student IDs: 921119898, 921572181, 922379312, 922894099
 * GitHub Name: copbrick
 * Group Name: Team DALJ
 * Project: Basic File System
 *
 * File: freeSpaceManager.h
 *
 * Description: Header file for free space manager
 *
 *
 *
 **************************************************************/
#ifndef FREESPACEMANAGER_H
#define FREESPACEMANAGER_H

int initFreeSpaceManager(int totalBlocks, int blockSize);
int findFreeBlocks(int requestedBlocks);
void setBit(unsigned char *freeSpaceManager, int blockNumber,int isFree);
int checkBit(unsigned char * freeSpaceManager,int bitIndex);
void freeBlocks(unsigned char * freeSpaceManger,int startIndex,int numberOfBlocks);

void printBitMap(unsigned char * freeSpaceManager);

#endif