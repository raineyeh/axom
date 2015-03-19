
#include "DataBuffer.hpp"
#include "DataGroup.hpp"

namespace DataStoreNS
{


DataBuffer::DataBuffer( const IDType uid ) :
    m_uid(uid),
    m_stringDescriptor(),
    m_ViewContainer(),
    m_data(nullptr),
    m_dataShape(),
    m_dataType(rtTypes::undefined),
    m_memblob()
{}

DataBuffer::DataBuffer( const IDType uid,
                        const std::string& stringDescriptor ) :
    m_uid(uid),
    m_stringDescriptor(stringDescriptor),
    m_ViewContainer(),
    m_data(nullptr),
    m_dataShape(),
    m_dataType(rtTypes::undefined),
    m_memblob()
{}

DataBuffer::DataBuffer(const DataBuffer& source ) :
    m_uid(source.m_uid),
    m_stringDescriptor(source.m_stringDescriptor),
    m_ViewContainer(source.m_ViewContainer),
    m_data(source.m_data),
    m_dataShape(source.m_dataShape),
    m_dataType(source.m_dataType),
    m_memblob(source.m_memblob)
{
}


DataBuffer::~DataBuffer()
{
}



DataBuffer* DataBuffer::Allocate()
{
  if ( m_dataShape.m_dimensions != nullptr && m_dataType!=rtTypes::undefined )
  {
    std::size_t size = 1;
    for (int dim = 0; dim < m_dataShape.m_numDimensions; ++dim)
    {
      size *= m_dataShape.m_dimensions[dim];
    }
    m_memblob.resize( size * rtTypes::sizeofType(m_dataType) );
    m_data = m_memblob.data();
  }
  else
  {
    throw std::exception();
  }

  ReconcileDataViews();

  return this;
}



void DataBuffer::AddDataView( DataView* dataView )
{
  m_ViewContainer.insert( dataView );
}


void DataBuffer::RemoveDataView( DataView* dataView )
{
  m_ViewContainer.erase( dataView );
}

void DataBuffer::ReconcileDataViews()
{
  for( ViewContainerType::iterator iterView=m_ViewContainer.begin() ;
       iterView != m_ViewContainer.end() ; ++iterView )
  {
    (*iterView)->ReconcileWithBuffer();
  }
}

} /* namespace Datastore */
