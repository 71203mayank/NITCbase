#include "BlockAccess.h"

#include <cstring>


RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op){

    // get the previous search index of the relation relId from the relation cache
    RecId prevRecId;
    RelCacheTable::getSearchIndex(relId,&prevRecId);

    // let block and slot denote the record id of the record beign currently checked
    int block, slot;
    // if the current search index record is invalid
    if(prevRecId.block == -1 && prevRecId.slot == -1){
        // it means no hits from the previous search, search should start from the first record itself
        // get the first record block of the relation from the relation cache using getRelCatEntry()
        RelCatEntry relCatEntry;
        RelCacheTable::getRelCatEntry(relId, &relCatEntry);

        // block = first record block of the relation
        // slot = 0
        block = relCatEntry.firstBlk;
        slot = 0;

    }
    else{
        // it means there is a hit from the previous search, should start from the record next to the search index record
        block = prevRecId.block;
        slot = prevRecId.slot + 1;
    }

    /*
        Now the following code will search for the next record in the relation
        that satisfies the given condition
        We will start from the record id (block, slot) and iterate over the
        remaining records of the relation
    */
    while( block != -1){
        // create a RecBuffer object for the block
        RecBuffer recBuffer(block);

        // get the record with id (block,slot) using RecBuffer::getRecord()
        // get header of the block using RecBuffer::getHeader()
        // get slot map of the block using RecBuffer::getSlotMap()
        struct HeadInfo head;
        recBuffer.getHeader(&head);
        Attribute record[head.numAttrs];
        recBuffer.getRecord(record,slot);
        unsigned char slotMap[head.numSlots];
        recBuffer.getSlotMap(slotMap);

        // if slot >= the number of slots per block
        if(slot >= head.numSlots){
            // update block = right block of block
            // update slot = 0
            block = head.rblock;
            slot = 0;
            continue;
        }
        /*
            my notes: if it is end of the record in a relation, this 
            loop will continue to reach the end of the block,
            eventually block = head.rblock = -1 and loop will terminate.
        */

        // if slot is free skip the loop
        // i.e. check if slot'th entry in slot map of block constains SLOT_UNOCCUPIED
        if(slotMap[slot] == SLOT_UNOCCUPIED){
            slot++;
            continue;
        }

        // now compare record's attribute value to the given attrVal as below:
        /*
            firstly get the attribute offset for the attrName attribute
            from the attribute cache entry of the relation using getAttrCatEntry
        */
        /* use the attribute offset to get the value of the attribute from
           current record */
        AttrCatEntry attrCatEntry;
        AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatEntry);

        Attribute currentAttrVal = record[attrCatEntry.offset];
        
        // store the difference b/w the attributes.
        int cmpVal = compareAttrs(currentAttrVal, attrVal, attrCatEntry.attrType);

        /* Next task is to check whether this record satisfies the given condition.
           It is determined based on the output of previous comparison and
           the op value received.
           The following code sets the cond variable if the condition is satisfied.
        */
        if(
            (op == NE && cmpVal != 0) ||
            (op == LT && cmpVal < 0) ||
            (op == LE && cmpVal <=0) ||
            (op == EQ && cmpVal == 0) ||
            (op == GT && cmpVal > 0) ||
            (op == GE && cmpVal >= 0)
        ){
            /*
                set the search index in the relation cache as
                the record id of the record that satisfies the given condition
                (use RelCacheTable::setSearchIndex function)
            */
            RecId recid = {block,slot};
            RelCacheTable::setSearchIndex(relId, &recid);

            return recid;
        }

        slot++;
    }

    // no record in the relation with Id relid satisfies the given condition
    return RecId{-1,-1};
}