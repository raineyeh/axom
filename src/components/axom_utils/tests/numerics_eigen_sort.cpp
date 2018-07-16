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

// Axom includes
#include "axom/Types.hpp"            // for AXOM_NULLPTR
#include "axom_utils/Matrix.hpp"     // for numerics::Matrix
#include "axom_utils/eigen_sort.hpp" // for eigen_sort()

// gtest includes
#include "gtest/gtest.h"

// namespace aliases
namespace numerics = axom::numerics;

//------------------------------------------------------------------------------
// UNIT TESTS
//------------------------------------------------------------------------------
TEST( numerics_eigen_sort, null_eigenvalues_vector )
{
  constexpr bool ZERO_COPY = true;

  // STEP 1: construct a fake matrix to for this test
  int matrix_data[ 9 /* 3 x 3 */] = {
    1, 2, 3,   // column 1
    4, 5, 6,   // column 2
    7, 8, 9    // column 3
  };
  numerics::Matrix< int > M( 3, 3, matrix_data, ZERO_COPY );

  int* l = AXOM_NULLPTR;
  EXPECT_FALSE( numerics::eigen_sort( l, M ) );
}

//------------------------------------------------------------------------------
TEST( numerics_eigen_sort, sort_eigenvalues_and_eigenvectors )
{
  constexpr bool ZERO_COPY = true;
  constexpr int N = 3;

  // STEP 0: construct a fake vector of eigenvalues
  int l[ 3 ] = { 3, 2, 1 };

  // STEP 1: construct a fake matrix to for this test
  int matrix_data[ 9 /* 3 x 3 */] = {
    1, 2, 3,   // column 1
    4, 5, 6,   // column 2
    7, 8, 9    // column 3
  };
  numerics::Matrix< int > M( N, N, matrix_data, ZERO_COPY );

  // STEP 2: constructed sorted matrix of eigen vectors
  int sorted_matrix_data[ 9 /* 3 x 3 */ ] = {
    7, 8, 9,    // column 1
    4, 5, 6,    // column 2
    1, 2, 3     // column 3
  };
  numerics::Matrix< int > sortedM( N, N, sorted_matrix_data, ZERO_COPY );

  // STEP 3: call eigen_sort
  EXPECT_TRUE( numerics::eigen_sort( l, M ) );

  // STEP 4: check eigen values & eigen vectors
  for ( int i=0 ; i < N ; ++i )
  {
    // test eigen values
    EXPECT_EQ( l[ i ], i+1 );

    // test eigen vectors
    const int* eigen_vector          = M.getColumn( i );
    const int* expected_eigen_vector = sortedM.getColumn( i );
    for ( int j=0 ; j < N ; ++j )
    {
      EXPECT_EQ( eigen_vector[ j ], expected_eigen_vector[ j ] );
    }

  }

  // STEP 5: check calling eigen_sort() on already sorted data
  EXPECT_TRUE( numerics::eigen_sort( l, M ) );

  // STEP 6: check eigen values and eigen vectors again
  for ( int i=0 ; i < N ; ++i )
  {
    // test eigen values
    EXPECT_EQ( l[ i ], i+1 );

    // test eigen vectors
    const int* eigen_vector          = M.getColumn( i );
    const int* expected_eigen_vector = sortedM.getColumn( i );
    for ( int j=0 ; j < N ; ++j )
    {
      EXPECT_EQ( eigen_vector[ j ], expected_eigen_vector[ j ] );
    }

  }

}

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();
  return result;
}
