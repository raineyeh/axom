//
// conduit/test/test1.cpp
//   Treat a conduit Node as a DataGroup.
//

#include "DataGroupConduit.hpp"

int main(int argc, char *argv[])
{
    conduit::uint32   a_val  = 10;
    conduit::uint32   b_val  = 20;
    conduit::float64  c_val  = 30.0;

    conduit::Node n;
    n["a"] = a_val;
    n["b"] = b_val;
    n["c"] = c_val;

    auto dsgrp = new DataStore::DataGroupConduit("node", &n);
#if 0
    auto dsnodea = dsgrp->GetDataObject("a");
    auto a2 = *(dsnodea->GetData<Datastore::uint32*>());
    assert(a_val == a2);
#endif

    return 0;
}
