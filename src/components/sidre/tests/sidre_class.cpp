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

#include <vector>

#include "sidre/sidre.hpp"

using asctoolkit::sidre::DataBuffer;
using asctoolkit::sidre::DataGroup;
using asctoolkit::sidre::DataStore;
using asctoolkit::sidre::DataView;

using namespace conduit;


namespace classtest
{

class Class1
{
public:

  Class1() { }

  explicit Class1(size_t len)
  {
    m_idata = std::vector<int>(len);

    for (size_t ii = 0 ; ii < len ; ++ii)
    {
      m_idata[ii] = 3 * ii;
    }
  }

  std::vector<int>& getIData() { return m_idata; }

  void copyToGroup(DataGroup * gp)
  {
    gp->createExternalView("idata", &m_idata[0],
                           DataType::c_int(m_idata.size()));
  }

  void copyFromGroup(DataGroup * gp)
  {
    DataView * iview = gp->getView("idata");
    size_t ilen = iview->getBuffer()->getNode().schema().total_bytes() /
                  sizeof(CONDUIT_NATIVE_INT);
    m_idata = std::vector<int>(ilen);

    int * g_idata = iview->getNode().as_int_ptr();
    for (size_t ii = 0 ; ii < ilen ; ++ii)
    {
      m_idata[ii] = g_idata[ii];
    }
  }

  void checkState(const Class1& class1)
  {
    checkState( &(class1.m_idata[0]) );
  }

  void checkState(DataGroup * gp)
  {
    int * idata_chk =
      gp->getView("idata")->getNode().as_int_ptr();
    checkState(idata_chk);
  }

private:

  void checkState(const int * tidata)
  {
    for (size_t ii = 0 ; ii < m_idata.size() ; ++ii)
    {
      EXPECT_EQ(m_idata[ii], tidata[ii]);
    }
  }

  std::vector<int> m_idata;
};

class Class2
{
public:

  Class2() { }

  explicit Class2(size_t len)
  {
    m_idata = std::vector<int>(len);
    m_ddata = std::vector<double>(len);
    m_class1 = Class1(len);

    for (size_t ii = 0 ; ii < len ; ++ii)
    {
      m_idata[ii] = ii;
      m_ddata[ii] = 2.0 * m_idata[ii];
    }
  }

  std::vector<int>& getIData() { return m_idata; }
  std::vector<double>& getDData() { return m_ddata; }
  Class1& getClass1() { return m_class1; }

  void copyToGroup(DataGroup * gp)
  {
    gp->createExternalView("idata", &m_idata[0],
                           DataType::c_int(m_idata.size()));
    gp->createExternalView("ddata", &m_ddata[0],
                           DataType::c_double(m_ddata.size()));

    DataGroup * gp1 = gp->createGroup("myclass1");

    m_class1.copyToGroup(gp1);
  }

  void copyFromGroup(DataGroup * gp)
  {
    DataView * iview = gp->getView("idata");
    size_t ilen = iview->getBuffer()->getNode().schema().total_bytes() /
                  sizeof(CONDUIT_NATIVE_INT);
    m_idata = std::vector<int>(ilen);

    int * g_idata = iview->getNode().as_int_ptr();
    for (size_t ii = 0 ; ii < ilen ; ++ii)
    {
      m_idata[ii] = g_idata[ii];
    }

    DataView * dview = gp->getView("ddata");
    size_t dlen = dview->getBuffer()->getNode().schema().total_bytes() /
                  sizeof(CONDUIT_NATIVE_DOUBLE);
    m_ddata = std::vector<double>(dlen);

    double * g_ddata = dview->getNode().as_double_ptr();
    for (size_t ii = 0 ; ii < dlen ; ++ii)
    {
      m_ddata[ii] = g_ddata[ii];
    }

    DataGroup * gp1 = gp->getGroup("myclass1");
    m_class1.copyFromGroup(gp1);
  }

  void checkState(const Class2& class2)
  {
    checkState( &(class2.m_idata[0]), &(class2.m_ddata[0]) );
    m_class1.checkState( class2.m_class1 );
  }

  void checkState(DataGroup * gp)
  {
    int * idata_chk =
      gp->getView("idata")->getNode().as_int_ptr();
    double * ddata_chk =
      gp->getView("ddata")->getNode().as_double_ptr();
    checkState(idata_chk, ddata_chk);

    DataGroup * gp1 = gp->getGroup("myclass1");
    m_class1.checkState(gp1);
  }

private:

  void checkState(const int * tidata, const double * tddata)
  {
    for (size_t ii = 0 ; ii < m_idata.size() ; ++ii)
    {
      EXPECT_EQ(m_idata[ii], tidata[ii]);
    }
    for (size_t ii = 0 ; ii < m_ddata.size() ; ++ii)
    {
      EXPECT_EQ(m_ddata[ii], tddata[ii]);
    }
  }

  std::vector<int> m_idata;
  std::vector<double> m_ddata;
  Class1 m_class1;
};


} // end namespace classtest

//------------------------------------------------------------------------------
// Test copying class data to group hierarchy
//------------------------------------------------------------------------------
TEST(sidre_class, class_to_group)
{
  using namespace classtest;

  const size_t len = 13;

  Class2 myclass2(len);

  EXPECT_EQ(myclass2.getIData().size(), len);
  EXPECT_EQ(myclass2.getDData().size(), len);
  EXPECT_EQ(myclass2.getClass1().getIData().size(), len);

  myclass2.checkState(myclass2);

  DataStore * ds   = new DataStore();
  DataGroup * root = ds->getRoot();

  DataGroup * c2group = root->createGroup("myclass2");
  myclass2.copyToGroup(c2group);

  ds->print();

  EXPECT_EQ(c2group->getNumViews(), 2u);
  EXPECT_EQ(c2group->getNumGroups(), 1u);

  myclass2.checkState(c2group);

  delete ds;
}

//------------------------------------------------------------------------------
// Test save/load class data using group hierarchy
//------------------------------------------------------------------------------
TEST(sidre_class, save_load_class_to_group)
{
  using namespace classtest;

  const size_t len = 21;

  Class2 myclass2(len);

  DataStore * ds   = new DataStore();
  DataGroup * root = ds->getRoot();

  DataGroup * c2group = root->createGroup("myclass2");
  myclass2.copyToGroup(c2group);

  EXPECT_EQ(c2group->getNumViews(), 2u);
  EXPECT_EQ(c2group->getNumGroups(), 1u);

  myclass2.checkState(c2group);

  ds->print();

  ds->getRoot()->save("out_save_load_class_to_group", "conduit");


  DataStore * ds2 = new DataStore();
  ds2->getRoot()->load("out_save_load_class_to_group","conduit");

  ds2->print();

  DataGroup * load_myclass2 = ds2->getRoot()->getGroup("myclass2");
  Class2 load_class2;

  load_class2.copyFromGroup(load_myclass2);

  myclass2.checkState(load_class2);

  delete ds;
  delete ds2;
}
