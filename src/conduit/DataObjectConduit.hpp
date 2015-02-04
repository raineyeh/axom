//
// DataObjectConduit.hpp - Access conduit data via datastore API
//

#include "DataObject.hpp"
#include "conduit.h"

namespace DataStore
{

class DataObjectConduit : DataObject
{
public:
    DataObjectConduit(const char *name, conduit::Node *node) :
	DataObject(name, NULL, NULL), m_node(node)
    { }

private:
    conduit::Node *m_node;

};


}
