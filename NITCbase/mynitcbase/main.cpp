#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
  /* Initialize the Run Copy of Disk */
  // Disk disk_run;
  // StaticBuffer buffer;
  // OpenRelTable cache;

  // return FrontendInterface::handleFrontend(argc, argv);

  Disk disk_run;

  unsigned char buffer[BLOCK_SIZE];
  Disk::readBlock(buffer, 7000);
  char message[] = "hello";

  //memcpy(to*, from*, size)
  memcpy(buffer + 20, message, 6);
  Disk::writeBlock(buffer, 7000);

  unsigned char buffer2[BLOCK_SIZE];
  char message2[6];
  Disk::readBlock(buffer2, 7000);
  memcpy(message2, buffer2 + 20, 6);
  cout<<message2<<endl;

  //Exercise: Reading form block allocation map
  unsigned char buffer3[BLOCK_SIZE];
  char message3[10];
  Disk::readBlock(buffer3,0);
  memcpy(message3,buffer3,10);
  // cout<<message3<<endl;
  for(auto x : message3){
    cout<<(int)x<<" ";
  }
  return 0;
  

}