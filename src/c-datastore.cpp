/*
 * datastore.c - C API for datastore.
 */

#include "datastore.h"
#include "DatastoreInterface.hpp"
#include "DataGroup.hpp"

extern "C" {

  DS_object *DS_create_datastore(const char *name)
  {
    DataStore::DataGroup* const myDS1 = DataStore::CreateDataStore(name);
    
    return (DS_object *) myDS1;
  }

  DS_object *DS_create_datagroup(DS_object *dg, const char *name)
  {
    DataStore::DataGroup* group = static_cast<DataStore::DataGroup *>(dg)->CreateDataGroup(name);
    return (DS_object *) group;
  }

  const char *DS_get_name(DS_object *obj)
  {
    return static_cast<DataStore::DataObject *>(obj)->Name().c_str();
  }


}
