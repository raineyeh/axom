/*******************************************************************************************************************************
 * An example algorithm for saving problem state data to a file, using the datastore for identifying and holding the data.
 *
 * This example uses a simple design, where a fresh 'restart' group is created before each dump.  It also assumes that a
 * physics package wants to write out all the data it is keeping in the datastore.
 *
 * This example also shows how a client code that is used to C++ references can convert from the pointers returned by the
 * datastore API.
 ******************************************************************************************************************************/

#include "../src/Types.hpp"
#include "../src/DataGroup.hpp"
#include "../src/DataBuffer.hpp"
#include "../src/DataStore.hpp"

class PhysicsPackage
{
   public:

      PhysicsPackage(sidre::DataGroup& group):mDataGroup(group) {}

      void setup()
      {
         sidre::DataGroup& subgroup = *mDataGroup.createGroup("physicsB");

         sidre::DataView& dataview = *subgroup.createViewAndBuffer("variable1");
         sidre::DataBuffer& buffer = *dataview.getBuffer();
         buffer.Declare(DataType::float64(100));
         buffer.Allocate();
         conduit::float64* data_ptr1 = buffer.GetNode().as_float64_ptr();
         // how do you get number of entries in conduit node? (ie, number of float64's)?
         for (size_t i=0; i < 100; ++i)
         {
            data_ptr1[i] = i;
         }

         dataview = *subgroup.createViewAndBuffer("variable2");
         buffer = *dataview.getBuffer();
         buffer.Declare(DataType::float64(100));
         buffer.Allocate();
         conduit::float64* data_ptr2 = buffer.GetNode().as_float64_ptr();
         // how do you get number of entries in conduit node? (ie, number of float64's)?
         for (size_t i=0; i < 100; ++i)
         {
            data_ptr2[i] = 100-i;
         }
         
         dataview = *subgroup.createViewAndBuffer("dependentVariable");
         buffer = *dataview.getBuffer();
         buffer.Declare(DataType::float64(100));
         buffer.Allocate();
         conduit::float64* data_ptr3 = buffer.GetNode().as_float64_ptr();
         // how do you get number of entries in conduit node? (ie, number of float64's)?
         for (size_t i=0; i < 100; ++i)
         {
            data_ptr3[i] = data_ptr1[i] * data_ptr2[i];
         }

      }

      void saveState(sidre::DataGroup& group)
      {
         // Since the package wants to save all it's data as-is, it can just copy over it's group (with views) to the restart group.
         // It assumes a fresh, empty restart group is provided.
         group.copyGroup( &mDataGroup );
      }

   private:
      sidre::DataGroup& mDataGroup;

};
   
class StateFile
{
   public:
      // Iterates over everything in provided tree and adds it to file
      void save(sidre::DataGroup& group)
      {
         
         // Iterate over all groups and views to exercise needed API calls.
         // A real code would follow up by writing each item to file.
      }
      
      // Read everything from file into group.
      void restore(sidre::DataGroup& group)
      {
         // Restore state data back into group, exercising needed API calls.
         // A real code would read in each item from file first.
      }

      void close() {}

};

int main(void)
{

   // Create datastore and problem state data.
   sidre::DataStore datastore;
   sidre::DataGroup& rootGroup = *datastore.getRoot();

   // Create a sub-tree for restart data.
   sidre::DataGroup& restartGroup = *rootGroup.createGroup("restart");

   // Create example physics package that will use datastore.
   PhysicsPackage physics(rootGroup);

   // Tell package to populate datastore with it's problem data.
   physics.setup();

   // Tell physics package to populate the 'restart' group with data it wants to save.
   physics.saveState( restartGroup );

   // Give 'restart' data group to another component, responsible for writing it out to file.
   StateFile file;
   file.save( restartGroup );
   file.close();

   // Clean up restart tree
   rootGroup.destroyGroup("restart");

   return 0;
}


