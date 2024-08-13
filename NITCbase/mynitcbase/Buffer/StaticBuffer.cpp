#include "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];

StaticBuffer::StaticBuffer(){
    // initialise all blocks as free
    for(int bufferIndex = 0; bufferIndex<BUFFER_CAPACITY; bufferIndex++){
        metainfo[bufferIndex].free = true;
    }
}

/*
    At this stage, we are not writing back from the buffer to the disk since we are
    not modifying the buffer. So, we will define an empty destructor for now, In
    subsequent stages, we will implement the write-back functionality here.
*/
StaticBuffer::~StaticBuffer(){}

int StaticBuffer::getFreeBuffer(int blockNum){
    if(blockNum < 0 || blockNum >= DISK_BLOCKS){
        return E_OUTOFBOUND;
    }

    int allocatedBuffer;

    // iterate through all the blocks in the StaticBuffer
    // find the first free block in the buffer (check metainfo)
    // assign allocatedBuffer = index of the free block
    for(int i = 0; i < BUFFER_CAPACITY ; i++){
        if(metainfo[i].free == true){
            allocatedBuffer = i;
            break;
        }
    }

    metainfo[allocatedBuffer].free = false;
    metainfo[allocatedBuffer].blockNum = blockNum;

    return allocatedBuffer;
}

/*
    Get the buffer index where a particular block is stored
    or E_BLOCKNOTINBUFFER otherwise
*/
int StaticBuffer::getBufferNum(int blockNum){
    // check if block is valid (b/w 0 and DISK_BLOCKS)
    if(blockNum < 0 || blockNum >= DISK_BLOCKS){
        return E_OUTOFBOUND;
    }

    // if found, return the bufferIndex which corresponds to blockNum
    for(int i = 0; i < BUFFER_CAPACITY; i++){
        if(metainfo[i].blockNum == blockNum && metainfo[i].free == false){
            return i;
        }
    }

    return E_BLOCKNOTINBUFFER;
}

