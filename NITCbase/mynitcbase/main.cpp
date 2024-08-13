#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
  /*
  Disk disk_run;
  StaticBuffer buffer;

  //Create objects for the relation catalog and attribute catalog
  RecBuffer relCatBuffer(RELCAT_BLOCK);
  RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

  HeadInfo relCatHeader;
  HeadInfo attrCatHeader;

  // load the headers of both the blocks into relCatHeader and attrCatHeader
  // will be implementing these function in future stages
  relCatBuffer.getHeader(&relCatHeader);
  attrCatBuffer.getHeader(&attrCatHeader);
  for(int i = 0; i < relCatHeader.numEntries; i++){

    Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog
    relCatBuffer.getRecord(relCatRecord, i);
    printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);

    for(int j = 0; j < attrCatHeader.numEntries; j++){

      //Declaring attrCatRecord and loading the attribute catalog entry into it
      Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
      attrCatBuffer.getRecord(attrCatRecord,j);

      // if attribute catalog entry corresponds to the current relation
      if(strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal , relCatRecord[RELCAT_REL_NAME_INDEX].sVal) == 0){
        const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER? "NUM" : "STR";
        printf(" %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
      }
    }
    printf("\n");
  }
  
  return 0;
  */

  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;

  RelCatEntry relEntry;
  AttrCatEntry attrEntry;
  // RELCAT_RELID = 0, ATTRCAT_RELID = 1
  for(int i = 0; i<3; i++){
    RelCacheTable::getRelCatEntry(i,&relEntry);
    printf("Relation: %s\n",relEntry.relName);

    for(int j = 0; j<=5 ; j++){
      AttrCacheTable::getAttrCatEntry(i,j,&attrEntry);
      printf("  %s: %s\n", attrEntry.attrName, (attrEntry.attrType == NUMBER)? "NUM" : "STR");
    }
    printf("\n");
  }

  /* FOR MODIFICATION */
  // this is directly branched from stage 1, it doesnot have STUDENTS created,
  // create student relation and then the outer loop make it i<3

  return 0;
  

}