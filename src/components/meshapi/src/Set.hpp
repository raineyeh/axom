/**
 * \file Set.h
 *
 * \brief Basic API for a set of entities in a simulation
 *
 */

#ifndef MESHAPI_SET_H_
#define MESHAPI_SET_H_

#include <cstddef>
#include <vector>

#include "common/Utilities.hpp"
#include "meshapi/Utilities.hpp"

namespace asctoolkit{
namespace meshapi{


  /**
   * \class Set
   *
   * \brief An indexed set (a tuple) of entities in a simulation
   *
   * A container class for a set of entities in a simulation.
   * Each entity has an index.
   *
   * Examples of sets include:
   * <ol>
   *  <li> Mesh elements: vertices, edges, faces, cells
   *  <li> Subzonal elements: sides, corners, finite element degrees of freedom
   *  <li> Boundary elements: external surfaces and springs
   *  <li> Elements of a space partition: e.g. Domains in a block structured mesh, leaf nodes of an octree/kd-tree
   *  <li> AMR bricks / tiles
   *  <li> Thread ids, MPI ranks, warps, thread groups, etc...
   *  <li> particles
   *  <li> boundary conditions
   *  <li> ...
   * </ol>
   *
   * Examples of subsets include:
   * <ol>
   *  <li> Regions
   *  <li> Ghost cells -- send, receive
   *  <li> Boundary cells -- external surface
   * </ol>
   *
   * Note: Elements of a set do not necessarily need explicit indices.
   * E.g. if we have a contiguous range of elements (or slices of contiguous ranges),
   * they can be implicitly encoded.
   *
   * Thus, we can have
   * <ol>
   *  <li> Implicit indexes -- all we need here is a size operator
   *  <li> Sliced indices -- here we need the dimension and the striding
   *  <li> Explicit indices -- for a subset, we need the indices with respect to some other indexing scheme
   * </ol>
   *
   * The interface is for constant access to the elements.
   */
  class Set
  {
  public:
    typedef MeshIndexType                 SetIndex;         // Index into a set
    typedef MeshSizeType                 SetPosition;      // Position in which we are indexing
    typedef MeshSizeType                  SizeType;         // Size of a set


  public:
      // Set () {}
      virtual ~Set () {}


      /**
       * \brief Random access to the entities of the set
       * @param idx The index of the desired element
       * @return The value of the element at the given position
       * \pre idx must be less than the number of elements in the set ( size() )
       * \note How are we planning to handle indexes that are out or range (accidentally)?
       *       How are we planning to handle indexes that are intentionally out of range
       *       (e.g. to indicate a problem, or a missing element etc..)?
       */
      virtual SetIndex at(SetPosition) const  =0;

      /**
       * \brief Get the number of entities in the set
       * @return The number of entities in the set.
       */
      virtual SizeType size() const      =0;

      /**
       * \brief Determines if the Set is a Subset of another set.
       * @return true if the set is a subset of another set, otherwise false.
       */
      virtual bool isSubset() const       =0;

      /**
       * @return A pointer to the parent set.  NullSet / NULL if there is no parent.
       */
      virtual const Set* parentSet() const           =0;

      /**
       * \brief Checks whether the set is valid.
       * \return true if the underlying indices are valid, false otherwise.
       */
      virtual bool isValid(bool verboseOutput = false)  const    =0;

#if 0
      //Possible other useful functions
      void reset(size_type) { throw NotImplementedException(); }
#endif


  private:
      /**
       * \brief Utility function to verify that the given SetPosition is in a valid range.
       */
      virtual void verifyPosition(SetPosition) const =0;
  };



  /**
   * \brief General equality operator for two sets.
   * \details Two sets are considered equal if they have the same number of elements, and their ordered indices agree.
   */
  inline bool operator==(Set const& set1, Set const& set2)
  {
      typedef Set::SetPosition SetPosition;
      typedef Set::SizeType SizeType;

      SizeType const numElts = set1.size();

      // Sets are different if they have a different size
      if(set2.size() != numElts)
          return false;

      // Otherwise, compare the indices element wise
      for(SetPosition pos= SetPosition(); pos < static_cast<SetPosition>(numElts); ++pos)
      {
          if(set1.at(pos) != set2.at(pos))
              return false;
      }
      return true;
  }
  /**
   * \brief Set inequality operator
   */
  inline bool operator!=(Set const& set1, Set const& set2){ return !(set1 == set2);}


} // end namespace meshapi
} // end namespace asctoolkit

#endif //  MESHAPI_SET_H_
