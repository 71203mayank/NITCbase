#include "Schema.h"

#include <cmath>
#include <cstring>

int Schema::openRel(char relName[ATTR_SIZE]){
    int ret = OpenRelTable::openRel(relName);

    // OpenRelTable::openRel() will send rel-id [0,12].
    //If relation is not open, any error code will return negative integer.
    if(ret >= 0){
        return SUCCESS;
    }

    return ret;
}

int Schema::closeRel(char relName[ATTR_SIZE]){
    // if relation in relation catalog or attribute catalog, the not permited to close
    if( strcmp(relName,RELCAT_RELNAME) == 0 || strcmp(relName,ATTRCAT_RELNAME) == 0 ){
        return E_NOTPERMITTED;
    }

    int relId = OpenRelTable::getRelId(relName);
    if( relId == E_RELNOTOPEN){
        return E_RELNOTOPEN;
    }

    return OpenRelTable::closeRel(relId);

}
