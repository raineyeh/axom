// Copyright (c) 2017-2019, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef AXOM_CUDA_EXEC_HPP_
#define AXOM_CUDA_EXEC_HPP_

#include "axom/config.hpp"
#include "axom/core/memory_management.hpp"

#include "RAJA/RAJA.hpp"
#include "umpire/Umpire.hpp"

#ifndef RAJA_ENABLE_CUDA
#error *** CUDA_EXEC requires a CUDA enabled RAJA ***
#endif

#if !defined(UMPIRE_ENABLE_CUDA) && !defined(UMPIRE_ENABLE_UM)
#error *** CUDA_EXEC requires a CUDA enabled UMPIRE with UM support ***
#endif

namespace axom
{

/*!
 * \brief Indicates parallel execution on the GPU with CUDA.
 * \tparam BLOCK_SIZE the number of CUDA threads in a block.
 */
template < int BLOCK_SIZE >
struct CUDA_EXEC{ };

/*!
 * \brief execution_space traits specialization for CUDA_EXEC
 * \tparam BLOCK_SIZE the number of CUDA threads to launch
 */
template < int BLOCK_SIZE >
struct execution_space< CUDA_EXEC< BLOCK_SIZE > >
{
  using loop_policy   = RAJA::cuda_exec< BLOCK_SIZE >;

  /* *INDENT-OFF* */
  using loop2d_policy =
      RAJA::KernelPolicy<
            RAJA::statement::CudaKernelFixed< 256,
              RAJA::statement::For<1, RAJA::cuda_block_x_loop,
                RAJA::statement::For<0, RAJA::cuda_thread_x_loop,
                  RAJA::statement::Lambda<0>
                >
              >
            >
          >;

  using loop3d_policy =
      RAJA::KernelPolicy<
        RAJA::statement::CudaKernelFixed< 256,
          RAJA::statement::For<2, RAJA::cuda_block_x_loop,
            RAJA::statement::For<1, RAJA::cuda_block_y_loop,
              RAJA::statement::For<0, RAJA::cuda_thread_x_loop,
                RAJA::statement::Lambda<0>
              >
            >
          >
        >
      >;
  /* *INDENT-ON* */

  using reduce_policy = RAJA::cuda_reduce;
  using atomic_policy = RAJA::cuda_atomic;
  using sync_policy   = RAJA::cuda_synchronize;

  static constexpr bool valid() noexcept { return true; };
  static constexpr char* name() noexcept { return (char*)"[CUDA_EXEC]"; };
  static int allocatorID() noexcept
  { return axom::getResourceAllocatorID(umpire::resource::Unified); };

};

} /* namespace axom */

#endif /* AXOM_CUDA_EXEC_HPP_ */
