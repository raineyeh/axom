/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and
 * further review from Lawrence Livermore National Laboratory.
 */

#include "gtest/gtest.h"

#include "sidre/sidre.hpp"

using asctoolkit::sidre::SidreLength;
using asctoolkit::sidre::TypeID;
using asctoolkit::sidre::DataBuffer;
using asctoolkit::sidre::DataGroup;
using asctoolkit::sidre::DataStore;
using asctoolkit::sidre::DataView;
using asctoolkit::sidre::IndexType;
using asctoolkit::sidre::InvalidIndex;
using asctoolkit::sidre::nameIsValid;
using asctoolkit::sidre::indexIsValid;
using asctoolkit::sidre::DataType;

// API coverage tests
// Each test should be documented with the interface functions being tested

//------------------------------------------------------------------------------
// getName()
//------------------------------------------------------------------------------
TEST(sidre_group,get_name)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();
  DataGroup * group = root->createGroup("test");

  EXPECT_TRUE(group->getName() == std::string("test") );

  DataGroup * group2 = root->getGroup("foo");
  EXPECT_TRUE(group2 == ATK_NULLPTR);
}

//------------------------------------------------------------------------------
// getNameWithPath()
//------------------------------------------------------------------------------
TEST(sidre_group,get_name_with_path)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();

  DataGroup * group =
    root->createGroup("test1")->createGroup("test2")->createGroup("test3");
  DataGroup * group2 = root->getGroup("test1/test2/test3");

  EXPECT_EQ(group, group2);

  // Now verify that code will not create missing groups.
  // TODO - improve error handling so this isn't fatal.
//  DataGroup * group3 = root->createGroup("testa")->createGroup("testb")->createGroup("testc");
//  DataGroup * group_bad = root->getGroup("testa/BAD/testc");

//  (void)group3;

//  EXPECT_EQ(group_bad, root->getGroup("testa") );

  delete ds;

}


//------------------------------------------------------------------------------
// getParent()
//------------------------------------------------------------------------------
TEST(sidre_group,get_parent)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();
  DataGroup * parent = root->createGroup("parent");
  DataGroup * child = parent->createGroup("child");

  EXPECT_TRUE( child->getParent() == parent );

  delete ds;
}

//------------------------------------------------------------------------------
// Verify getDatastore()
//------------------------------------------------------------------------------
TEST(sidre_group,get_datastore)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();
  DataGroup * group = root->createGroup("parent");

  EXPECT_TRUE( group->getDataStore() == ds );

  DataStore const * const_ds = group->getDataStore();
  EXPECT_TRUE( const_ds == ds );

  delete ds;
}

//------------------------------------------------------------------------------
// Verify getGroup()
//------------------------------------------------------------------------------
TEST(sidre_group,get_group)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();

  DataGroup * parent = root->createGroup("parent");
  DataGroup * child = parent->createGroup("child");
  EXPECT_TRUE( child->getParent() == parent );

  EXPECT_TRUE( parent->getGroup("child") == child );
  // check error condition
  EXPECT_TRUE( parent->getGroup("non-existant group") == ATK_NULLPTR );

  delete ds;
}

//------------------------------------------------------------------------------
// Verify getView()
//------------------------------------------------------------------------------
TEST(sidre_group,get_view)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();

  DataGroup * parent = root->createGroup("parent");
  DataView * view = parent->createView("view");

  EXPECT_TRUE( parent->getView("view") == view );

  // check error condition
  EXPECT_TRUE( parent->getView("non-existant view") == ATK_NULLPTR );

  delete ds;
}
//------------------------------------------------------------------------------
// Verify getViewWithPath()
//------------------------------------------------------------------------------
TEST(sidre_group,get_view_with_path)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();

  DataView * view =
    root->createGroup("group1")->createGroup("group2")->createView("view1");
  DataView * view2 = root->getView("group1/group2/view1");

  EXPECT_EQ( view, view2 );

  delete ds;
}


//------------------------------------------------------------------------------
// Verify getViewName(), getViewIndex()
//------------------------------------------------------------------------------
TEST(sidre_group,get_view_names_and_indicies)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();

  DataGroup * parent = root->createGroup("parent");
  DataView * view1 = parent->createView("view1");
  DataView * view2 = parent->createView("view2");

  EXPECT_EQ(parent->getNumViews(), 2u);

  IndexType idx1 = parent->getViewIndex("view1");
  IndexType idx2 = parent->getViewIndex("view2");

  const std::string& name1 = parent->getViewName(idx1);
  const std::string& name2 = parent->getViewName(idx2);

  EXPECT_EQ(name1, std::string("view1"));
  EXPECT_EQ(view1->getName(), name1);

  EXPECT_EQ(name2, std::string("view2"));
  EXPECT_EQ(view2->getName(), name2);

  // check error conditions
  IndexType idx3 = parent->getViewIndex("view3");
  EXPECT_TRUE(idx3 == InvalidIndex);

  const std::string& name3 = parent->getViewName(idx3);
  EXPECT_TRUE(name3.empty());
  EXPECT_FALSE(nameIsValid(name3));

  delete ds;
}

//------------------------------------------------------------------------------
// Verify getFirstValidViewIndex, getNextValidGroupIndex
//------------------------------------------------------------------------------
TEST(sidre_group,get_first_and_next_view_index)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();

  DataGroup * parent = root->createGroup("parent");
  DataView * view1 = parent->createView("view1");
  DataView * view2 = parent->createView("view2");

  DataGroup * emptyGroup = root->createGroup("emptyGroup");

  EXPECT_EQ(parent->getNumViews(), 2u);

  IndexType idx1 = parent->getFirstValidViewIndex();
  IndexType idx2 = parent->getNextValidViewIndex(idx1);

  const std::string& name1 = parent->getViewName(idx1);
  const std::string& name2 = parent->getViewName(idx2);

  EXPECT_EQ(name1, std::string("view1"));
  EXPECT_EQ(view1->getName(), name1);

  EXPECT_EQ(name2, std::string("view2"));
  EXPECT_EQ(view2->getName(), name2);

  // check error conditions
  IndexType badidx1 = emptyGroup->getFirstValidViewIndex();
  IndexType badidx2 = emptyGroup->getNextValidViewIndex(badidx1);

  EXPECT_TRUE(badidx1 == InvalidIndex);
  EXPECT_TRUE(badidx2 == InvalidIndex);

  delete ds;
}
//------------------------------------------------------------------------------
// Verify getGroupName(), getGroupIndex()
//------------------------------------------------------------------------------
TEST(sidre_group,get_group_name_index)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();

  DataGroup * parent = root->createGroup("parent");
  DataGroup * group1 = parent->createGroup("group1");
  DataGroup * group2 = parent->createGroup("group2");

  EXPECT_EQ(parent->getNumGroups(), 2u);

  IndexType idx1 = parent->getGroupIndex("group1");
  IndexType idx2 = parent->getGroupIndex("group2");

  const std::string& name1 = parent->getGroupName(idx1);
  const std::string& name2 = parent->getGroupName(idx2);

  EXPECT_EQ(name1, std::string("group1"));
  EXPECT_EQ(group1->getName(), name1);

  EXPECT_EQ(name2, std::string("group2"));
  EXPECT_EQ(group2->getName(), name2);

  // check error conditions
  IndexType idx3 = parent->getGroupIndex("group3");
  EXPECT_TRUE(idx3 == InvalidIndex);

  const std::string& name3 = parent->getGroupName(idx3);
  EXPECT_TRUE(name3.empty());
  EXPECT_FALSE(nameIsValid(name3));

  delete ds;
}

//------------------------------------------------------------------------------
// createView()
// createViewAndAllocate()
// destroyView()
// destroyViewAndData()
// hasView()
//------------------------------------------------------------------------------
TEST(sidre_group,create_destroy_has_view)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();
  DataGroup * group = root->createGroup("parent");

  DataView * view = group->createView("view");
  EXPECT_TRUE( group->getParent() == root );
  EXPECT_FALSE( view->hasBuffer() );

  EXPECT_TRUE( group->hasView("view") );
  // try creating view again, should be a no-op.
  EXPECT_TRUE( group->createView("view") == ATK_NULLPTR );

  group->destroyView("view");
  // destroy already destroyed group.  Should be a no-op, not a failure
  group->destroyView("view");

  EXPECT_FALSE( group->hasView("view") );

  // try api call that specifies specific type and length
  group->createViewAndAllocate( "viewWithLength1",
                                asctoolkit::sidre::FLOAT_ID, 50 );

  // error condition check - try again with duplicate name, should be a no-op
  EXPECT_TRUE( group->createViewAndAllocate( "viewWithLength1",
                                             asctoolkit::sidre::FLOAT64_ID,
                                             50 ) == ATK_NULLPTR );
  group->destroyViewAndData("viewWithLength1");
  EXPECT_FALSE( group->hasView("viewWithLength1") );

  EXPECT_TRUE( group->createViewAndAllocate( "viewWithLengthBadLen",
                                             asctoolkit::sidre::FLOAT64_ID,
                                             -1 ) == ATK_NULLPTR );

  // try api call that specifies data type in another way
  group->createViewAndAllocate( "viewWithLength2", DataType::float64(50) );
  EXPECT_TRUE( group->createViewAndAllocate( "viewWithLength2",
                                             DataType::float64(
                                               50) ) == ATK_NULLPTR );
  // destroy this view using index
  group->destroyViewAndData( group->getFirstValidViewIndex() );

  delete ds;
}

//------------------------------------------------------------------------------
// createGroup()
// destroyGroup()
// hasGroup()
//------------------------------------------------------------------------------
TEST(sidre_group,create_destroy_has_group)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();
  DataGroup * group = root->createGroup("group");
  EXPECT_TRUE( group->getParent() == root );

  EXPECT_TRUE( root->hasGroup("group") );

  root->destroyGroup("group");
  EXPECT_FALSE( root->hasGroup("group") );

  // should be a no-op, not a failure
  root->destroyGroup("group");

  DataGroup * group2 = root->createGroup("group2");
  // shut up compiler about unused variable
  (void)group2;
  root->destroyGroup( root->getFirstValidGroupIndex() );

  delete ds;
}

//------------------------------------------------------------------------------
TEST(sidre_group,group_name_collisions)
{
  DataStore * ds = new DataStore();
  DataGroup * flds = ds->getRoot()->createGroup("fields");
  flds->createView("a");

  EXPECT_TRUE(flds->hasView("a"));

  // attempt to create duplicate group name

  DataGroup * badGroup = ds->getRoot()->createGroup("fields");
  EXPECT_TRUE( badGroup == ATK_NULLPTR );

  // check error condition
  // attempt to create duplicate view name.
  EXPECT_TRUE(flds->createView("a") == ATK_NULLPTR);

  delete ds;
}
//------------------------------------------------------------------------------
#if 0
TEST(sidre_group,view_copy_move)
{
  DataStore * ds = new DataStore();
  DataGroup * flds = ds->getRoot()->createGroup("fields");

  flds->createViewAndAllocate("i0", DataType::c_int());
  flds->createViewAndAllocate("f0", DataType::c_float());
  flds->createViewAndAllocate("d0", DataType::c_double());

  flds->getView("i0")->setScalar(1);
  flds->getView("f0")->setScalar(100.0);
  flds->getView("d0")->setScalar(3000.0);

  EXPECT_TRUE(flds->hasView("i0"));
  EXPECT_TRUE(flds->hasView("f0"));
  EXPECT_TRUE(flds->hasView("d0"));

  // test moving a view from flds to sub
  flds->createGroup("sub")->moveView(flds->getView("d0"));
  // flds->print();
  EXPECT_FALSE(flds->hasView("d0"));
  EXPECT_TRUE(flds->hasGroup("sub"));
  EXPECT_TRUE(flds->getGroup("sub")->hasView("d0"));

  // check the data value
  double * d0_data =  flds->getGroup("sub")->getView("d0")->getData();
  EXPECT_NEAR(d0_data[0],3000.0,1e-12);

  // test copying a view from flds to sub
  flds->getGroup("sub")->copyView(flds->getView("i0"));

  // flds->print();

  EXPECT_TRUE(flds->hasView("i0"));
  EXPECT_TRUE(flds->getGroup("sub")->hasView("i0"));

  // we expect the data pointers to be the same
  int * i0_ptr = flds->getView("i0")->getData();
  int * sub_io0_ptr = flds->getGroup("sub")->getView("i0")->getData();
  EXPECT_EQ(i0_ptr, sub_io0_ptr);

  delete ds;
}
#endif
//------------------------------------------------------------------------------
#if 0
TEST(sidre_group,groups_move_copy)
{
  DataStore * ds = new DataStore();
  DataGroup * flds = ds->getRoot()->createGroup("fields");

  DataGroup * ga = flds->createGroup("a");
  DataGroup * gb = flds->createGroup("b");
  DataGroup * gc = flds->createGroup("c");

  ga->createViewAndAllocate("i0", DataType::c_int());
  gb->createViewAndAllocate("f0", DataType::c_float());
  gc->createViewAndAllocate("d0", DataType::c_double());

  ga->getView("i0")->setScalar(1);
  gb->getView("f0")->setScalar(100.0);
  gc->getView("d0")->setScalar(3000.0);

  // check that all sub groups exist
  EXPECT_TRUE(flds->hasGroup("a"));
  EXPECT_TRUE(flds->hasGroup("b"));
  EXPECT_TRUE(flds->hasGroup("c"));

  // move "b" to a child of "sub"
  flds->createGroup("sub")->moveGroup(gb);

  // flds->print();

  EXPECT_TRUE(flds->hasGroup("a"));
  EXPECT_TRUE(flds->hasGroup("sub"));
  EXPECT_TRUE(flds->hasGroup("c"));

  EXPECT_EQ(flds->getGroup("sub")->getGroup("b"),gb);

  delete ds;
}
#endif
//------------------------------------------------------------------------------
TEST(sidre_group,create_destroy_view_and_buffer2)
{
  DataStore * const ds = new DataStore();
  DataGroup * const grp = ds->getRoot()->createGroup("grp");

  std::string viewName1("viewBuffer1");
  std::string viewName2("viewBuffer2");

  DataView * view1 = grp->createViewAndAllocate(viewName1,
                                                asctoolkit::sidre::INT_ID, 1);
  DataView * view2 = grp->createViewAndAllocate(viewName2,
                                                asctoolkit::sidre::INT_ID, 1);

  EXPECT_TRUE(grp->hasView(viewName1));
  EXPECT_EQ( grp->getView(viewName1), view1 );

  EXPECT_TRUE(grp->hasView(viewName2));
  EXPECT_EQ( grp->getView(viewName2), view2 );

  IndexType const bufferId1 = view1->getBuffer()->getIndex();

  grp->destroyViewAndData(viewName1);

  EXPECT_FALSE(grp->hasView(viewName1));
  EXPECT_EQ(ds->getNumBuffers(), 1u);

  DataBuffer const * const buffer1 = ds->getBuffer(bufferId1);
  EXPECT_TRUE( buffer1 == ATK_NULLPTR );

  DataView const * const view3 = grp->createView("viewBuffer3");
  grp->destroyViewsAndData();
  // should be no-op
  grp->destroyViewsAndData();
  // shut up compiler about unused variable
  (void)view3;

  delete ds;
}


//------------------------------------------------------------------------------
TEST(sidre_group,create_destroy_alloc_view_and_buffer)
{
  DataStore * const ds = new DataStore();
  DataGroup * const grp = ds->getRoot()->createGroup("grp");

  std::string const viewName1 = "viewBuffer1";
  std::string const viewName2 = "viewBuffer2";

  // use create + alloc convenience methods
  // this one is the DataType & method
  DataView * const view1 = grp->createViewAndAllocate(viewName1,
                                                      DataType::c_int(10));

  EXPECT_TRUE(grp->hasView(viewName1));
  EXPECT_EQ( grp->getView(viewName1), view1 );

  int * v1_vals = view1->getData();

  for(int i=0 ; i<10 ; i++)
  {
    v1_vals[i] = i;
  }

  EXPECT_EQ(view1->getNumElements(), 10u);
  EXPECT_EQ(view1->getTotalBytes(), static_cast<asctoolkit::sidre::SidreLength>(10 * sizeof(int)));

  grp->destroyViewAndData(viewName1);

  delete ds;
}

//------------------------------------------------------------------------------
TEST(sidre_group,create_view_of_buffer_with_schema)
{
  DataStore * ds = new DataStore();
  DataGroup * root = ds->getRoot();
  // use create + alloc convenience methods
  // this one is the DataType & method
  DataView * base =  root->createViewAndAllocate("base", DataType::c_int(10));
  int * base_vals = base->getData();
  for(int i=0 ; i<10 ; i++)
  {
    if(i < 5)
    {
      base_vals[i] = 10;
    }
    else
    {
      base_vals[i] = 20;
    }
  }

  DataBuffer * base_buff = base->getBuffer();

  // create two views into this buffer
  //
  // view for the first 5 values
  root->createView("sub_a", base_buff)->apply(DataType::c_int(5));

  int * sub_a_vals = root->getView("sub_a")->getData();

  for(int i=0 ; i<5 ; i++)
  {
    EXPECT_EQ(sub_a_vals[i], 10);
  }

  delete ds;
}




//------------------------------------------------------------------------------
TEST(sidre_group,save_restore_simple)
{
  DataStore * ds = new DataStore();
  DataGroup * flds = ds->getRoot()->createGroup("fields");

  DataGroup * ga = flds->createGroup("a");

  ga->createView("i0")->allocate(DataType::c_int());

  ga->getView("i0")->setScalar(1);

  EXPECT_TRUE(ds->getRoot()->hasGroup("fields"));
  EXPECT_TRUE(ds->getRoot()->getGroup("fields")->hasGroup("a"));
  EXPECT_TRUE(ds->getRoot()->getGroup("fields")->getGroup("a")->hasView("i0"));


  ds->getRoot()->save("out_sidre_group_save_restore_simple","conduit");

  //ds->print();

  DataStore * ds2 = new DataStore();

  ds2->getRoot()->load("out_sidre_group_save_restore_simple","conduit");

  //ds2->print();

  flds = ds2->getRoot()->getGroup("fields");
  // check that all sub groups exist
  EXPECT_TRUE(flds->hasGroup("a"));
  int testvalue = flds->getGroup("a")->getView("i0")->getData();
  EXPECT_EQ(testvalue,1);

  //ds2->print();

  delete ds;
  delete ds2;

}

//------------------------------------------------------------------------------
TEST(sidre_group,save_restore_complex)
{
  DataStore * ds = new DataStore();
  DataGroup * flds = ds->getRoot()->createGroup("fields");

  DataGroup * ga = flds->createGroup("a");
  DataGroup * gb = flds->createGroup("b");
  DataGroup * gc = flds->createGroup("c");

  ga->createViewAndAllocate("i0", DataType::c_int());
  gb->createViewAndAllocate("f0", DataType::c_float());
  gc->createViewAndAllocate("d0", DataType::c_double());

  ga->getView("i0")->setScalar(1);
  // Be careful on floats.  If you just hand it 100.0, the compiler will assume you want a double.
  // Either cast the value to float, or be explicit on the template argument.
  gb->getView("f0")->setScalar( 100.0f );
  // this would have worked equally well also.
  // gb->getView("f0")->setScalar<float>(100.0);
  gc->getView("d0")->setScalar(3000.00);

  // check that all sub groups exist
  EXPECT_TRUE(flds->hasGroup("a"));
  EXPECT_TRUE(flds->hasGroup("b"));
  EXPECT_TRUE(flds->hasGroup("c"));

  //ds->print();

  ds->getRoot()->save("out_sidre_group_save_restore_complex","conduit");

  DataStore * ds2 = new DataStore();


  ds2->getRoot()->load("out_sidre_group_save_restore_complex","conduit");

  flds = ds2->getRoot()->getGroup("fields");
  // check that all sub groups exist
  EXPECT_TRUE(flds->hasGroup("a"));
  EXPECT_TRUE(flds->hasGroup("b"));
  EXPECT_TRUE(flds->hasGroup("c"));

  EXPECT_EQ(flds->getGroup("a")->getView("i0")->getData<int>(),1);
  EXPECT_NEAR(flds->getGroup("b")->getView("f0")->getData<float>(),100.0,
              1e-12);
  EXPECT_NEAR(flds->getGroup("c")->getView(
                "d0")->getData<double>(),3000.0, 1e-12);

  //ds2->print();

  delete ds;
  delete ds2;

}
