#include "Frontend.h"

#include <cstring>
#include <iostream>

int Frontend::create_table(char relname[ATTR_SIZE], int no_attrs, char attributes[][ATTR_SIZE],
                           int type_attrs[]) {
  // Schema::createRel
  return Schema::createRel(relname, no_attrs, attributes, type_attrs);
}

int Frontend::drop_table(char relname[ATTR_SIZE]) {
  // Schema::deleteRel
  return Schema::deleteRel(relname);
}

int Frontend::open_table(char relname[ATTR_SIZE]) {
  // Schema::openRel
  return Schema::openRel(relname);
}

int Frontend::close_table(char relname[ATTR_SIZE]) {
  // Schema::closeRel
  return Schema::closeRel(relname);
}

int Frontend::alter_table_rename(char relname_from[ATTR_SIZE], char relname_to[ATTR_SIZE]) {
  // Schema::renameRel
  return Schema:: renameRel(relname_from, relname_to);
}

int Frontend::alter_table_rename_column(char relname[ATTR_SIZE], char attrname_from[ATTR_SIZE],
                                        char attrname_to[ATTR_SIZE]) {
  // Schema::renameAttr
  return Schema:: renameAttr(relname, attrname_from, attrname_to);
}

int Frontend::create_index(char relname[ATTR_SIZE], char attrname[ATTR_SIZE]) {
  // Schema::createIndex
  return Schema::createIndex(relname, attrname);
}

int Frontend::drop_index(char relname[ATTR_SIZE], char attrname[ATTR_SIZE]) {
  // Schema::dropIndex
  return Schema::dropIndex(relname, attrname);
}

int Frontend::insert_into_table_values(char relname[ATTR_SIZE], int attr_count, char attr_values[][ATTR_SIZE]) {
  // Algebra::insert
  return Algebra::insert(relname, attr_count, attr_values);
}

int Frontend::select_from_table(char relname_source[ATTR_SIZE], char relname_target[ATTR_SIZE]) {
  // Algebra::project
  return Algebra::project(relname_source, relname_target);
}

int Frontend::select_attrlist_from_table(char relname_source[ATTR_SIZE], char relname_target[ATTR_SIZE],
                                         int attr_count, char attr_list[][ATTR_SIZE]) {
  // Algebra::project
  return Algebra::project(relname_source, relname_target, attr_count, attr_list);
}

int Frontend::select_from_table_where(char relname_source[ATTR_SIZE], char relname_target[ATTR_SIZE],
                                      char attribute[ATTR_SIZE], int op, char value[ATTR_SIZE]) {
  // Algebra::select
  return Algebra::select(relname_source, relname_target, attribute, op, value);
}

int Frontend::select_attrlist_from_table_where(char relname_source[ATTR_SIZE], char relname_target[ATTR_SIZE],
                                               int attr_count, char attr_list[][ATTR_SIZE],
                                               char attribute[ATTR_SIZE], int op, char value[ATTR_SIZE]) {
  // Algebra::select + Algebra::project??
  // printf("hello");
   char relname_temp[ATTR_SIZE] = TEMP;
  //  printf("in frontend: %s", relname_temp);
  
  
  int ret = Algebra::select(relname_source, relname_temp, attribute, op, value);
  if (ret != SUCCESS)
    return ret;

  
  int tempRelId = OpenRelTable::openRel(relname_temp);
  if (tempRelId < 0 || tempRelId >= MAX_OPEN) {
    Schema::deleteRel(relname_temp);
    return tempRelId;
  }

  ret = Algebra::project(relname_temp, relname_target, attr_count, attr_list);
  OpenRelTable::closeRel(tempRelId);
  Schema::deleteRel(relname_temp);
  // printf("Ret val %d",ret);
  return ret;
}

int Frontend::select_from_join_where(char relname_source_one[ATTR_SIZE], char relname_source_two[ATTR_SIZE],
                                     char relname_target[ATTR_SIZE],
                                     char join_attr_one[ATTR_SIZE], char join_attr_two[ATTR_SIZE]) {
  // Algebra::join
  return Algebra::join(relname_source_one, relname_source_two, relname_target, join_attr_one, join_attr_two);
}

int Frontend::select_attrlist_from_join_where(char relname_source_one[ATTR_SIZE], char relname_source_two[ATTR_SIZE],
                                              char relname_target[ATTR_SIZE],
                                              char join_attr_one[ATTR_SIZE], char join_attr_two[ATTR_SIZE],
                                              int attr_count, char attr_list[][ATTR_SIZE]) {
  // Algebra::join + project
  // we first use join and get temp relation, then select desired attributes using project() from temp to target relation
  

  // call join() method and create a temporary target name TEMP
  char tempRelName[ATTR_SIZE] = TEMP;
  int ret = Algebra::join(relname_source_one, relname_source_two, tempRelName, join_attr_one, join_attr_two);

  /*
    TEMP contains all the attributes of the source relations except the join attribute of the 
    second source relation
  */

  if(ret != SUCCESS){
    return ret;
  }

  // Open TEMP, if fails delete it
  int tempRelId = OpenRelTable::openRel(tempRelName);
  if(tempRelId < 0){
    Schema::deleteRel(tempRelName);
    return tempRelId;
  }

  // call project() [2nd function]
  ret = Algebra::project(tempRelName, relname_target, attr_count, attr_list);

  OpenRelTable::closeRel(tempRelId);
  Schema::deleteRel(tempRelName);

  return ret;
}

int Frontend::custom_function(int argc, char argv[][ATTR_SIZE]) {
  // argc gives the size of the argv array
  // argv stores every token delimited by space and comma

  // implement whatever you desire

  return SUCCESS;
}