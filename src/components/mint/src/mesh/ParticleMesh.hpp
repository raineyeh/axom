/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Copyright (c) 2017-2018, Lawrence Livermore National Security, LLC.
 *
 * Produced at the Lawrence Livermore National Laboratory
 *
 * LLNL-CODE-741217
 *
 * All rights reserved.
 *
 * This file is part of Axom.
 *
 * For details about use and distribution, please read axom/LICENSE.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#ifndef MINT_PARTICLEMESH_HPP_
#define MINT_PARTICLEMESH_HPP_

#include "axom/Macros.hpp"   // for axom macros

#include "mint/config.hpp"   // for mint compile-time definitions
#include "mint/Mesh.hpp"     // for mint::Mesh base class

#include "slic/slic.hpp"     // for slic Macros

namespace axom
{

// Sidre Forward Declarations
namespace sidre
{
class Group;
}

namespace mint
{

// Mint Forward Declarations
class MeshCoordinates;

/*!
 * \brief Provides the ability to store and operate on a set of particles.
 *
 *  The ParticleMesh class derives from the top-level Mesh base class and
 *  provides the ability to store and operate on a collection of particles and
 *  associated data. Each particle has a position and may have associated
 *  scalar, vector and tensor fields.
 *
 *  A ParticleMesh object may be constructed using (a) native storage, (b)
 *  external storage, or, (c) from a Sidre blueprint conforming hierarchy:
 *
 *  * <b> Native Storage </b> <br />
 *
 *    When using native storage, the ParticleMesh object owns all memory
 *    associated with the particle data. The storage can grow dynamically as
 *    needed by the application, i.e., adding more particles. Once the
 *    ParticleMesh object goes out-of-scope, all memory associated with it is
 *    returned to the system.
 *
 *  * <b> External Storage </b> <br />
 *
 *    A ParticleMesh may also be constructed from external, user-supplied
 *    buffers. In this case, all memory associated with the particle data
 *    is owned by the caller. Consequently, the number of particles and
 *    associated data cannot grow dynamically.
 *
 *  * <b> Sidre </b> <br />
 *
 *    A ParticleMesh may also be constructed from a Sidre hierarchy that is
 *    conforming to the <a href="http://llnl-conduit.readthedocs.io/en/latest/">
 *    mesh blueprint </a> conventions. In this case, all operations are
 *    supported, including dynamically adding new particles and growing the
 *    associated storage. However, Sidre owns all the memory. Once the
 *    ParticleMesh object goes out-of-scope, the data remains persistent in
 *    Sidre.
 *
 * \see mint::Mesh
 */
class ParticleMesh : public Mesh
{

public:

  /*!
   * \brief Default constructor. Disabled.
   */
  ParticleMesh( ) = delete;

/// \name Native Storage Constructors
/// @{

  /*!
   * \brief Constructs a ParticleMesh instance of specified dimension that
   *  consists the specified number of particles.
   *
   * \param [in] dimension the ambient dimension of the particle mesh.
   * \param [in] numParticles the number of particles in this
   * \param [in] capacity max particle capacity (optional)
   *
   * \pre 1 <= dimension <= 3
   * \pre numParticles >= 0
   *
   * \post getNumParticles() == numParticles
   * \post getNumParticles() <= capacity()
   * \post hasSidreGroup() == false
   */
  ParticleMesh( int dimension,
                IndexType numParticles,
                IndexType capacity=USE_DEFAULT );

  /*!
   * \brief Constructs a ParticleMesh instance of specified dimension that
   *  consists the specified number of particles.
   *
   * \param [in] dimension the ambient dimension of the particle mesh.
   * \param [in] blockId the block ID.
   * \param [in] partId the partition ID.
   * \param [in] numParticles the number of particles in this
   * \param [in] capacity max particle capacity (optional)
   *
   * \pre 1 <= dimension <= 3
   * \pre numParticles >= 0
   *
   * \post getNumParticles() == numParticles
   * \post getNumParticles() <= capacity()
   * \post hasSidreGroup() == false
   */
  ParticleMesh( int dimension, int blockId, int partId,
                IndexType numParticles,
                IndexType capacity=USE_DEFAULT );
/// @}

/// \name External Storage Constructors
/// @{

  /*!
   * \brief Creates a ParticleMesh instance that points to the supplied external
   *  particle position buffers.
   *
   * \param [in] numParticles the number of particles in the supplied buffers.
   * \param [in] x pointer to the particle x-coordinate positions
   * \param [in] y pointer to the particle y-coordinate positions (optional)
   * \param [in] z pointer to the particle z-coordinate positions (optional)
   *
   * \note This constructor wraps the supplied particle posision buffers.
   *  Consequently, the resulting ParticleMesh object does not own the memory
   *  associated with the particle positions.
   *
   * \warning All calls to shrink(), append(), resize() and reserve will fail
   *  on a ParticleMesh instance that is constructed using this external
   * \note The supplied buffers must have sufficient storage f`numParticles`
   *
   * \pre x != AXOM_NULLPTR
   * \pre y != AXOM_NULLPTR, if dimension==2 || dimension==3
   * \pre z != AXOM_NULLPTR, if dimension==3
   * \pre numParticles >= 1
   *
   * \post 1 <= getDimension() <= 3
   * \post getNumParticles() == numParticles
   */
  ParticleMesh( IndexType numParticles,
                double* x,
                double* y=AXOM_NULLPTR,
                double* z=AXOM_NULLPTR    );

  /*!
   * \brief Creates a ParticleMesh instance that points to the supplied external
   *  particle position buffers.
   *
   * \param [in] blockId the block ID of this mesh instance.
   * \param [in] partId the partition ID of this mesh instance.
   * \param [in] numParticles the number of particles in the supplied buffers.
   * \param [in] x pointer to the particle x-coordinate positions
   * \param [in] y pointer to the particle y-coordinate positions (optional)
   * \param [in] z pointer to the particle z-coordinate positions (optional)
   *
   * \note This constructor wraps the supplied particle posision buffers.
   *  Consequently, the resulting ParticleMesh object does not own the memory
   *  associated with the particle positions.
   *
   * \warning All calls to shrink(), append(), resize() and reserve will fail
   *  on a ParticleMesh instance that is constructed using this external
   * \note The supplied buffers must have sufficient storage f`numParticles`
   *
   * \pre x != AXOM_NULLPTR
   * \pre y != AXOM_NULLPTR, if dimension==2 || dimension==3
   * \pre z != AXOM_NULLPTR, if dimension==3
   * \pre numParticles >= 1
   *
   * \post 1 <= getDimension() <= 3
   * \post getNumParticles() == numParticles
   */
  ParticleMesh( int blockId, int partId,
                IndexType numParticles,
                double* x,
                double* y=AXOM_NULLPTR,
                double* z=AXOM_NULLPTR    );
/// @}

/// \name Sidre Storage Constructors
/// @{

  /*!
   * \brief Creates a ParticleMesh instance from a Sidre group.
   *
   * \param [in] group pointer to the blueprint root group in Sidre
   * \param [in] topo the name of the associated topology (optional)
   *
   * \note The supplied group is expected to contain particle mesh data
   *  that is valid and conforming to the conventions described in the mesh
   *  <a href="http://llnl-conduit.readthedocs.io/en/latest/"> blueprint </a>.
   *
   * \note If a topology name is not provided, the implementation will construct
   *  a mesh based on the 1st topology group under the parent "topologies"
   *  group.
   *
   * \note When using this constructor, all data is owned by Sidre. Once the
   *  ParticleMesh object goes out-of-scope, the data remains persistent in
   *  Sidre.
   *
   * \pre group != AXOM_NULLPTR
   * \pre blueprint::validRootGroup( group )
   * \post hasSidreGroup() == true
   */
  /// @{

  ParticleMesh( sidre::Group* group,
                const std::string& topo );

  explicit ParticleMesh( sidre::Group* group );

  /// @}

  /*!
   * \brief Creates a ParticleMesh object on the specified Sidre group.
   *
   * \param [in] dimension the ambient dimension of the particle mesh.
   * \param [in] blockId the block ID of this mesh instance (optional).
   * \param [in] partId the partition ID of this mesh instance (optional).
   * \param [in] numParticles the number of particles this instance holds
   * \param [in] group pointer to a group in Sidre where
   * \param [in] topo the name of the associated topology (optional)
   * \param [in] coordset the name of the coordset group in Sidre (optional)
   * \param [in] capacity max particle capacity (optional).
   *
   * \note If a topology and coordset name is not provided, internal defaults
   *  will be used by the implementation.
   *
   * \note When using this constructor, all data is owned by Sidre. Once the
   *  ParticleMesh object goes out-of-scope, the data remains persistent in
   *  Sidre.
   *
   * \pre 1 <= dimension <= 3
   * \pre group != AXOM_NULLPTR
   * \pre group->getNumViews()==0
   * \pre group->getNumGroups()==0
   *
   * \post hasSidreGroup()==true
   */
  /// @{

  ParticleMesh( int dimension, int blockId, int partId,
                IndexType numParticles,
                sidre::Group* group,
                const std::string& topo,
                const std::string& coordset,
                IndexType capacity=USE_DEFAULT );

  ParticleMesh( int dimension,
                IndexType numParticles,
                sidre::Group* group,
                const std::string& topo,
                const std::string& coordset,
                IndexType capacity=USE_DEFAULT );

  ParticleMesh( int dimension, int blockId, int partId,
                IndexType numParticles,
                sidre::Group* group,
                IndexType capacity=USE_DEFAULT );

  ParticleMesh( int dimension,
                IndexType numParticles,
                sidre::Group* group,
                IndexType capacity=USE_DEFAULT );
  /// @}

/// @}

  /*!
   * \brief Destructor.
   */
  virtual ~ParticleMesh();

  /*!
   * \brief Returns the total number of particles in this mesh instance.
   * \return N the total number of particles.
   */
  inline IndexType getNumParticles() const
  { return m_num_nodes; }

  /*!
   * \brief The max particle capacity that this ParticleMesh instance can hold
   * \return N the particle capacity
   */
  inline IndexType capacity( ) const
  { return m_positions->capacity(); };

  /*!
   * \brief Returns pointer to the particle positions in the specified dimension
   *
   * \param[in] dim the specified dimension
   * \return coord pointer
   *
   * \pre 1 <= dim <= 3
   * \post coord != AXOM_NULLPTR
   */
  /// @{

  inline double* getParticlePositions( int dim )
  { return m_positions->getCoordinateArray( dim ); }

  inline const double* getParticlePositions( int dim ) const
  { return m_positions->getCoordinateArray( dim ); }

  /// @}

/// \name Append Methods
/// @{

  /*!
   * \brief Appends a new particle to the ParticleMesh
   *
   * \param [in] x the x-coordinate of the particle position
   * \param [in] y the y-coordinate of the particle position (valid in 2-D,3-D)
   * \param [in] z the z-coordinate of the particle position (valid in 3-D only)
   *
   * \post increments the number of particles by one.
   */
  /// @{

  inline void append( double x );
  inline void append( double x, double y );
  inline void append( double x, double y, double z );

  /// @}

/// @}

/// \name Size/Capacity Modifiers
/// @{

  /*!
   * \brief Increase the number of particles this ParticleMesh instance can hold
   * \param [in] newSize the number of particles this instance will now hold
   * \post getNumParticles() == newSize
   */
  inline void resize( IndexType newSize );

  /*!
   * \brief Increase the max particle capacity of this ParticleMesh instance
   * \param [in] newCapacity
   */
  inline void reserve( IndexType newCapacity );

  /*!
   * \brief Shrinks the max particle capacity to the actual number of particles.
   *
   * \post getNumParticles() == capacity()
   * \post f->getCapacity()==getNumParticles() for all particle fields.
   */
  inline void shrink( );

/// @}

private:

  /*!
   * \brief Helper method to initialize a ParticleMesh instance.
   * \note Called from the constructor.
   */
  void initialize( );

  /*!
   * \brief Helper method to check if the particle data is consistent
   * \return status true if the consistency checks pass, else, false.
   */
  bool checkConsistency( );

  MeshCoordinates* m_positions;

  DISABLE_COPY_AND_ASSIGNMENT(ParticleMesh);
  DISABLE_MOVE_AND_ASSIGNMENT(ParticleMesh);
};

//------------------------------------------------------------------------------
// IN-LINE METHOD IMPLEMENTATION
//------------------------------------------------------------------------------
inline void ParticleMesh::append( double x )
{
  SLIC_ASSERT( m_positions != AXOM_NULLPTR );
  SLIC_ERROR_IF( m_ndims != 1,
                 "ParticleMesh::append(x) is only valid in 1-D" );

  m_positions->append( x );
  m_mesh_fields[ NODE_CENTERED ]->resize( m_positions->numNodes( ) );
  m_num_nodes = m_num_cells = m_positions->numNodes( );

  SLIC_ASSERT( checkConsistency() );
}

//------------------------------------------------------------------------------
inline void ParticleMesh::append( double x, double y )
{
  SLIC_ASSERT( m_positions != AXOM_NULLPTR );
  SLIC_ERROR_IF( m_ndims != 2,
                "ParticleMesh::append(x,y) is only valid in 2-D" );

  m_positions->append( x, y );
  m_mesh_fields[ NODE_CENTERED ]->resize( m_positions->numNodes( ) );
  m_num_nodes = m_num_cells = m_positions->numNodes( );

  SLIC_ASSERT( checkConsistency() );
}

//------------------------------------------------------------------------------
inline void ParticleMesh::append( double x, double y, double z )
{
  SLIC_ASSERT( m_positions != AXOM_NULLPTR );
  SLIC_ERROR_IF( m_ndims != 3,
                "ParticleMesh::append(x,y,z) is only valid in 3-D" );

  m_positions->append( x, y, z );
  m_mesh_fields[ NODE_CENTERED ]->resize( m_positions->numNodes( ) );
  m_num_nodes = m_num_cells = m_positions->numNodes( );

  SLIC_ASSERT( checkConsistency() );
}

//------------------------------------------------------------------------------
inline void ParticleMesh::resize( IndexType newSize )
{
  SLIC_ASSERT( m_positions != AXOM_NULLPTR );
  m_positions->resize( newSize );
  m_mesh_fields[ NODE_CENTERED ]->resize( newSize );
  m_num_nodes = m_num_cells = m_positions->numNodes();
  SLIC_ASSERT( checkConsistency() );
}

//------------------------------------------------------------------------------
inline void ParticleMesh::reserve( IndexType newCapacity )
{
  SLIC_ASSERT( m_positions != AXOM_NULLPTR );
  m_positions->reserve( newCapacity );
  m_mesh_fields[ NODE_CENTERED ]->reserve( newCapacity );
}

//------------------------------------------------------------------------------
inline void ParticleMesh::shrink( )
{
  SLIC_ASSERT( m_positions != AXOM_NULLPTR );
  m_positions->shrink();
  m_mesh_fields[ NODE_CENTERED ]->shrink( );
}

} /* namespace mint */
} /* namespace axom */

#endif /* PARTICLEMESH_HXX_ */
