//
// DataGroupConduit.hpp - Access conduit data via datastore API
//

#include "DataGroup.hpp"
#include "conduit.h"

namespace DataStore
{

class DataGroupConduit : DataGroup
{
public:
    DataGroupConduit(const char *name, conduit::Node *node) :
	DataGroup(name, name, NULL), m_node(node)
    { }

private:
    conduit::Node *m_node;

};


}
