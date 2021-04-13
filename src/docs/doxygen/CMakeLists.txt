# Copyright (c) 2017-2021, Lawrence Livermore National Security, LLC and
# other Axom Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (BSD-3-Clause)
#------------------------------------------------------------------------------
# Doxygen documentation targets
#------------------------------------------------------------------------------

blt_add_doxygen_target( axom_doxygen )

set(AXOM_CUSTOM_TARGET_PREFIX CACHE STRING "")
set(axom_doxygen_target_name ${AXOM_CUSTOM_TARGET_PREFIX}build_doxygen_output_dir)

# Required directory for doxygen to generate output to 
add_custom_target( ${axom_doxygen_target_name} 
                   ALL 
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/html 
                 ) 

add_dependencies( axom_doxygen ${axom_doxygen_target_name} )
