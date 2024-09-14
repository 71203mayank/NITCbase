#include "BlockBuffer.h"

#include <cstdlib>
#include <cstring>


// BlockBuffer(int blockNum)
BlockBuffer :: BlockBuffer(int blockNum){
    // initialise this.blockNum with the argument
    this->blockNum = blockNum;
}

// RecBuffer(int blockNum), calls the parent class constructor
RecBuffer :: RecBuffer(int blockNum) : BlockBuffer :: BlockBuffer(blockNum) {}

/*
    Used to get the header of the block into the location pointed to by 'head'
    NOTE: This function expects the caller to allocate memory for 'head'
*/
int BlockBuffer :: getHeader(struct HeadInfo *head) {

    unsigned char *bufferPtr;
    int ret = loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret != SUCCESS){
        return ret;
    }

    // populate the numEntries, numAttrs and numSlots fields in *head
    memcpy(&head->numSlots, bufferPtr + 24, 4);
    memcpy(&head->numEntries, bufferPtr + 16, 4);
    memcpy(&head->numAttrs, bufferPtr + 20, 4);
    memcpy(&head->rblock, bufferPtr + 12, 4);
    memcpy(&head->lblock, bufferPtr + 8, 4);

    return SUCCESS;
}

/*
    Used to get the record at slot 'slotNum' into the array 'rec'
    NOTE: this function expects the caller to allocate memory for 'rec'
*/
int RecBuffer :: getRecord(union Attribute *rec, int slotNum){

    unsigned char *bufferPtr;
    int ret = loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret != SUCCESS){
        return ret;
    }

    struct HeadInfo head;

    // get the header using this.getHeader() function
    this->getHeader(&head);

    int attrCount = head.numAttrs;
    int slotCount = head.numSlots;

    /* record at slotNum will be at offset HEADER_SIZE + slotMapSize + (recordSize * slotNum)
        - each record will have size attrCount * ATTR_SIZE
        - slotMap will be of size slotCount
    */
    int recordSize = attrCount * ATTR_SIZE;
    unsigned char *slotPointer = bufferPtr + HEADER_SIZE + head.numSlots + (recordSize * slotNum);

    // load the record into the rec data structure
    memcpy(rec, slotPointer, recordSize);

    return SUCCESS;
}

/*  
    getSlotMap():
    used to get the slotmap from a record block
    NOTE: this functions expects the caller to allocate memory for "*slotmap"
*/
int RecBuffer::getSlotMap(unsigned char* slotMap){
    unsigned char* bufferPtr;

    // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr()
    int ret = loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret != SUCCESS){
        return ret;
    }

    struct HeadInfo head;
    this->getHeader(&head);
    int slotCount = head.numSlots;

    // get a pointer to the beginning of the slotmap in memory by offsetting HEADER_SIZE
    unsigned char* slotMapInBuffer = bufferPtr + HEADER_SIZE;

    // copy the values from 'slotMapInBuffer' to 'slotMap' (size is 'slotCount')
    memcpy(slotMap, slotMapInBuffer,slotCount);

    return SUCCESS;
}

/*
    Used to load a block to the buffer and get a pointer to it.
    NOTE: this function expects the caller to allocate memory for the argument
*/
int BlockBuffer :: loadBlockAndGetBufferPtr(unsigned char **buffPtr){
    // check whether the block is already present in the buffer using StaticBuffer.getBufferNum();
    int bufferNum = StaticBuffer :: getBufferNum(this->blockNum);

    // if buffer is present
        // set the timeStamp of the corresponding buffer at bufferMetaInfo to 0, rest increment by one
    if(bufferNum != E_BLOCKNOTINBUFFER){
        for(int bufferIndex = 0; bufferIndex < BUFFER_CAPACITY; bufferIndex++){
            if(bufferIndex == bufferNum){
                StaticBuffer::metainfo[bufferIndex].timeStamp = 0;
            }
            else{
                StaticBuffer::metainfo[bufferIndex].timeStamp += 1;
            }
        }
    }
    else{
        bufferNum = StaticBuffer :: getFreeBuffer(this->blockNum);
        if(bufferNum == E_OUTOFBOUND){
            return E_OUTOFBOUND;
        }

        Disk :: readBlock(StaticBuffer :: blocks[bufferNum], this->blockNum);
    }

    // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr
    *buffPtr = StaticBuffer :: blocks[bufferNum];

    return SUCCESS;
}


// compareAttrs()
int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType){
    double diff;
    if(attrType == STRING){
        diff = strcmp(attr1.sVal, attr2.sVal);
    }
    else{
        diff = attr1.nVal - attr2.nVal;
    }

    if(diff > 0){
        return 1;
    }
    else if(diff < 0){
        return -1;
    }
    else{
        return 0;
    }
}

/*
    RecBuffer::setRecord()
    Sets the slotNum th record entry of the block with the input record content
*/
int RecBuffer::setRecord(union Attribute *rec, int slotNum){
    unsigned char *bufferPtr;
    /*
        get the starting address of the buffer containing the block
        using loadBlockAndGetBufferPtr(&bufferPtr);
    */
    int res = BlockBuffer::loadBlockAndGetBufferPtr(&bufferPtr);

    if(res != SUCCESS){
        return res;
    }

    /* get the header of the block using the getHeader() */
    struct HeadInfo head;
    this->getHeader(&head);

    // get the number of attributes
    int attrCount = head.numAttrs;

    // get the number of slots in the block
    int slotCount = head.numSlots;

    // if input slotNum is not in the permitted range return E_OUTOFBOUND
    if(slotNum >= slotCount){
        return E_OUTOFBOUND;
    }

    /*
        Offset bufferPtr to point to the beginning of the record at required slot.
        The block contains the header, the slotmap, followed by all the records.
    */
    int recordSize = attrCount * ATTR_SIZE;
    unsigned char *slotPointer = bufferPtr + HEADER_SIZE + slotCount + (slotNum * recordSize);

    // copy the record from rec to the buffer using memcpy
    memcpy(slotPointer, rec, recordSize);

    // update the dirty bit using setDirtyBit()
    StaticBuffer::setDirtyBit(this->blockNum);

    return SUCCESS;
}