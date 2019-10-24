// Copyright (c) 2017-2019, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef AXOM_SEQ_EXEC_HPP_
#define AXOM_SEQ_EXEC_HPP_

#include "axom/config.hpp"
#include "axom/core/memory_management.hpp"

// RAJA includes
#ifdef AXOM_USE_RAJA
#include "RAJA/RAJA.hpp"
#endif

// Umpire includes
#ifdef AXOM_USE_UMPIRE
#include "umpire/Umpire.hpp"
#endif


namespace axom
{


/*!
 * \brief Indicates sequential execution on the CPU.
 */
struct SEQ_EXEC{ };

/*!
 * \brief execution_space traits specialization for SEQ_EXEC
 */
template < >
struct execution_space< SEQ_EXEC >
{

#ifdef AXOM_USE_RAJA
  using loop_policy   = RAJA::loop_exec;

  /* *INDENT-OFF* */
  using loop2d_policy =
      RAJA::KernelPolicy<
               RAJA::statement::For< 1, RAJA::loop_exec,   // j
                 RAJA::statement::For< 0, RAJA::loop_exec, // i
                   RAJA::statement::Lambda< 0 >
                 > // END i
               > // END j
            >; // END kernel

  using loop3d_policy =
      RAJA::KernelPolicy<
              RAJA::statement::For< 2, RAJA::loop_exec,       // k
                 RAJA::statement::For< 1, RAJA::loop_exec,    // j
                    RAJA::statement::For< 0, RAJA::loop_exec, // i
                       RAJA::statement::Lambda< 0 >
                    > // END i
                  > // END j
               > // END k
            >; // END kernel
  /* *INDENT-ON* */

  using reduce_policy = RAJA::loop_reduce;
  using atomic_policy = RAJA::loop_atomic;
#else
  using loop_policy   = void;
  using loop2d_policy = void;
  using loop3d_policy = void;

  using reduce_policy = void;
  using atomic_policy = void;
#endif

  using sync_policy = void;

  static constexpr bool valid() noexcept { return true; };
  static constexpr char* name() noexcept { return (char*)"[SEQ_EXEC]"; };
  static int allocatorID() noexcept
  {
#ifdef AXOM_USE_UMPIRE
    return axom::getResourceAllocatorID( umpire::resource::Host );
#else
    return 0;
#endif
  };

};

} /* namespace axom */

#endif /* AXOM_SEQ_EXEC_HPP_ */
