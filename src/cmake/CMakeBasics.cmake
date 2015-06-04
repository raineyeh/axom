###############################################################################
# Copyright (c) 2014, Lawrence Livermore National Security, LLC.
#
# Produced at the Lawrence Livermore National Laboratory
#
# LLNL-CODE-666778
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the disclaimer below.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the disclaimer (as noted below) in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the name of the LLNS/LLNL nor the names of its contributors may
#   be used to endorse or promote products derived from this software without
#   specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
# LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
###############################################################################

################################
# Standard Build Layout
################################

##
## Defines the layout of the build directory. Namely,
## it indicates the location where the various header files should go,
## where to store libraries (static or shared), the location of the
## bin directory for all executables and the location for fortran moudules.
##

## Set the path where all the header will be stored
 set(HEADER_INCLUDES_DIRECTORY
     ${PROJECT_BINARY_DIR}/include/
     CACHE PATH
     "Directory where all headers will go in the build tree"
     )
 include_directories(${HEADER_INCLUDES_DIRECTORY})

 ## Set the path where all the libraries will be stored
 set(LIBRARY_OUTPUT_PATH
     ${PROJECT_BINARY_DIR}/lib
     CACHE PATH
     "Directory where compiled libraries will go in the build tree"
     )

 ## Set the path where all the executables will go
 set(EXECUTABLE_OUTPUT_PATH
     ${PROJECT_BINARY_DIR}/bin
     CACHE PATH
     "Directory where executables will go in the build tree"
     )

 ## Set the Fortran module directory
 set(CMAKE_Fortran_MODULE_DIRECTORY
     ${PROJECT_BINARY_DIR}/lib
     CACHE PATH
     "Directory where all Fortran modules will go in the build tree"
     )

## Mark as advanced
mark_as_advanced(
     LIBRARY_OUTPUT_PATH
     EXECUTABLE_OUTPUT_PATH
     CMAKE_Fortran_MODULE_DIRECTORY
     )


################################
# Check if we want to build Fortran support.
################################
option(ENABLE_FORTRAN "Enables Fortran compiler support" ON)

if(ENABLE_FORTRAN)
    # if enabled but no fortran compiler, halt the configure
    if(CMAKE_Fortran_COMPILER)
        MESSAGE(STATUS  "Fortran support enabled. (ENABLE_FORTRAN == ON, Fortran compiler found.)")
    else()
        MESSAGE(FATAL_ERROR "Fortran support selected, but no Fortran compiler was found.")
    endif()    
else()
    MESSAGE(STATUS  "Fortran support disabled. (ENABLE_FORTRAN == OFF)")
endif()

################################
# Standard CMake Options
################################

include(ExternalProject)

option(BUILD_TESTING "Builds unit tests" ON)
if (BUILD_TESTING)

  ## add catch
  include_directories(${PROJECT_SOURCE_DIR}/TPL/catch/single_include)

  ## add google test
  add_subdirectory(${PROJECT_SOURCE_DIR}/TPL/gtest-1.7.0)
  set(GTEST_INCLUDES ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR}
            CACHE INTERNAL "GoogleTest include directories" FORCE)
  set(GTEST_LIBS gtest_main gtest
            CACHE INTERNAL "GoogleTest link libraries" FORCE)

  enable_testing()

endif()

## Enable ENABLE C++ 11 features
option(ENABLE_CXX11 "Enables C++11 features" OFF)
if (ENABLE_CXX11)
  # define a macro so the code can ifdef accordingly.
  add_definitions("-DUSE_CXX11")
endif()

## Choose static or shared libraries.
option(BUILD_SHARED_LIBS "Build shared libraries." OFF)

option(ENABLE_WARNINGS "Enable Compiler warnings." ON)
if(ENABLE_WARNINGS)

    # set the warning levels we want to abide by
    if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
        add_definitions(/W2)
    else()
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
            "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
            # using clang or gcc
            add_definitions(-Wall -Wextra -Werror)
        endif()
    endif()

endif()


#############################################
# Support extra compiler flags and defines
#############################################
#
# We don't try to use this approach for CMake generators that support 
# multiple configurations. See: CZ JIRA: ATK-45
#
if(NOT CMAKE_CONFIGURATION_TYPES)
    
    ######################################################
    # Add define we can use when debug builds are enabled
    ######################################################
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(-DATK_DEBUG)
    endif()

    ##########################################
    # Support Extra Flags for the C compiler.
    ##########################################
    if(EXTRA_C_FLAGS)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_C_FLAGS}")
    endif()

    # Extra Flags for the debug builds with the C compiler.
    if(EXTRA_C_FLAGS_DEBUG AND CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options("${EXTRA_C_FLAGS_DEBUG}")
    endif()

    # Extra Flags for the release builds with the C compiler.
    if(EXTRA_C_FLAGS_RELEASE AND CMAKE_BUILD_TYPE MATCHES RELEASE)
        add_compile_options("${EXTRA_C_FLAGS_RELEASE}")
    endif()

    #############################################
    # Support Extra Flags for the C++ compiler.
    #############################################
    if(EXTRA_CXX_FLAGS)
        add_compile_options("${EXTRA_CXX_FLAGS}")
    endif()

    # Extra Flags for the debug builds with the C++ compiler.
    if(EXTRA_CXX_FLAGS_DEBUG AND CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options("${EXTRA_CXX_FLAGS_DEBUG}")
    endif()

    # Extra Flags for the release builds with the C++ compiler.
    if(EXTRA_CXX_FLAGS_RELEASE AND CMAKE_BUILD_TYPE MATCHES RELEASE)
        add_compile_options("${EXTRA_CXX_FLAGS_RELEASE}")
    endif()

endif()

################################
# RPath Settings
################################

# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# the RPATH to be used when installing, but only if it's not a system directory
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

################################
#  macros
################################

##------------------------------------------------------------------------------
## - Adds a component to the build given the component's name and default state
##   (ON/OFF). This macro also adds an "option" so that the user can control
##   which components to build.
##------------------------------------------------------------------------------
macro(add_component)

    set(options)
    set(singleValueArgs COMPONENT_NAME DEFAULT_STATE )
    set(multiValueArgs)

    ## parse the arugments to the macro
    cmake_parse_arguments(arg
         "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})
    
    ## setup a cmake vars to capture sources added via our macros
    set("${arg_COMPONENT_NAME}_ALL_SOURCES" CACHE PATH "" FORCE)
        
    ## adds an option so that the user can control whether to build this
    ## component.
    option( ENABLE_${arg_COMPONENT_NAME}
            "Enables ${arg_component_name}"
            ${arg_DEFAULT_STATE})

    if ( ENABLE_${arg_COMPONENT_NAME} )
        add_subdirectory( ${arg_COMPONENT_NAME} )
    endif()

endmacro(add_component)

##------------------------------------------------------------------------------
## - Given a library target nane and a list of sources, this macros builds
##   a static or shared library according to a user-supplied BUILD_SHARED_LIBS
##   option.
##
## make_library(libtarget srcs)
##------------------------------------------------------------------------------
macro(make_library libtarget srcs)

   if ( BUILD_SHARED_LIBS )
      add_library(${libtarget} SHARED ${srcs})
   else()
      add_library(${libtarget} STATIC ${srcs})
   endif()

   if ( ENABLE_CXX11 )
      ## Note, this requires cmake 3.1 and above
      set_property(TARGET ${libtarget} PROPERTY CXX_STANDARD 11)
   endif()
    
    
   foreach(src ${srcs})
       if(IS_ABSOLUTE)
           list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${src}")
       else()
           list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
       endif()
   endforeach()
       
    set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )

endmacro(make_library)

##------------------------------------------------------------------------------
## - Make executable
##
## make_executable( EXECUTABLE_SOURCE source DEPENDS_ON dep1 dep2...)
##------------------------------------------------------------------------------
macro(make_executable)

   set(options)
   set(singleValueArgs EXECUTABLE_SOURCE)
   set(multiValueArgs DEPENDS_ON)

   ## parse the arugments to the macro
   cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})

    get_filename_component(exe_name ${arg_EXECUTABLE_SOURCE} NAME_WE)
    add_executable( ${exe_name} ${arg_EXECUTABLE_SOURCE} )
    target_link_libraries( ${exe_name} "${arg_DEPENDS_ON}" )

    if ( ENABLE_CXX11 )
      ## Note, this requires cmake 3.1 and above
      set_property(TARGET ${exe_name} PROPERTY CXX_STANDARD 11)
    endif()
    
        
    if(IS_ABSOLUTE)
        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${arg_EXECUTABLE_SOURCE}")
    else()
        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${arg_EXECUTABLE_SOURCE}")
    endif()

    set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )
    
endmacro(make_executable)

##------------------------------------------------------------------------------
## - Builds and adds a test which employs the Catch unit testing framework.
##
## add_gtest( TEST_SOURCE testX.cxx DEPENDS_ON dep1 dep2... )
##------------------------------------------------------------------------------
macro(add_gtest)

   set(options)
   set(singleValueArgs TEST_SOURCE)
   set(multiValueArgs DEPENDS_ON)

   ## parse the arguments to the macro
   cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

   get_filename_component(test_name ${arg_TEST_SOURCE} NAME_WE)
   add_executable( ${test_name} ${arg_TEST_SOURCE} )
   target_include_directories(${test_name} PRIVATE "${GTEST_INCLUDES}")
   target_link_libraries( ${test_name} "${GTEST_LIBS}" )
   target_link_libraries( ${test_name} "${arg_DEPENDS_ON}" )

    if ( ENABLE_CXX11 )
      ## Note, this requires cmake 3.1 and above
      set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 11)
    endif()

    add_test( NAME ${test_name}
              COMMAND ${test_name}
              WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
              )

    # add any passed source files to the running list for this project
    if(IS_ABSOLUTE)
        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${arg_TEST_SOURCE}")
    else()
          list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${arg_TEST_SOURCE}")
    endif()

    set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )
    
endmacro(add_gtest)

##------------------------------------------------------------------------------
## - Builds and adds a test which employs the Catch unit testing framework.
##
## add_catch_test( TEST_SOURCE testX.cxx DEPENDS_ON dep1 dep2... )
##------------------------------------------------------------------------------
macro(add_catch_test)

   set(options)
   set(singleValueArgs TEST_SOURCE)
   set(multiValueArgs DEPENDS_ON)

   ## parse the arguments to the macro
   cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

   get_filename_component(test_name ${arg_TEST_SOURCE} NAME_WE)
   add_executable( ${test_name} ${arg_TEST_SOURCE} )
   target_link_libraries( ${test_name} "${arg_DEPENDS_ON}" )

    if ( ENABLE_CXX11 )
      ## Note, this requires cmake 3.1 and above
      set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 11)
    endif()

    add_test( NAME ${test_name}
              COMMAND ${test_name}
              WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
              )

    # add any passed source files to the running list for this project
    if(IS_ABSOLUTE)
        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${arg_TEST_SOURCE}")
    else()
        list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${arg_TEST_SOURCE}")
    endif()

    set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )

endmacro(add_catch_test)


##------------------------------------------------------------------------------
## - Builds and adds a fortran based test.
##
## add_fortran_test( TEST_SOURCE testX.f DEPENDS_ON dep1 dep2... )
##------------------------------------------------------------------------------
macro(add_fortran_test)
    # only add the test if fortran is enabled
    if(ENABLE_FORTRAN)
       set(options)
       set(singleValueArgs TEST_SOURCE)
       set(multiValueArgs DEPENDS_ON)

       ## parse the arguments to the macro
       cmake_parse_arguments(arg
            "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

       get_filename_component(test_name ${arg_TEST_SOURCE} NAME_WE)
       add_executable( ${test_name} ${arg_TEST_SOURCE} )
       target_link_libraries( ${test_name} "${arg_DEPENDS_ON}" )

        set_target_properties(${test_name}  PROPERTIES Fortran_FORMAT "FREE")

        add_test( NAME ${test_name}
                  COMMAND ${test_name}
                  WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
                  )
       #TODO: we aren't tracking / grouping fortran sources. 
   endif()
endmacro(add_fortran_test)


##------------------------------------------------------------------------------
## - Adds a custom "copy_headers" target to copy the given list of headers to
##   the supplied destination directory.
##
## copy_headers_copy( hdrs dest )
##------------------------------------------------------------------------------
macro(copy_headers_target proj hdrs dest)

add_custom_target(copy_headers_${proj}
     COMMAND ${CMAKE_COMMAND}
             -DHEADER_INCLUDES_DIRECTORY=${dest}
             -DLIBHEADERS="${hdrs}"
             -P ${CMAKE_MODULE_PATH}/copy_headers.cmake

     DEPENDS
        ${hdrs}

     WORKING_DIRECTORY
        ${PROJECT_SOURCE_DIR}

     COMMENT
        "copy headers"
     )
     
     # add any passed source files to the running list for this project
     foreach(hdr ${hdrs})
         if(IS_ABSOLUTE)
             list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${hdr}")
         else()
             list(APPEND "${PROJECT_NAME}_ALL_SOURCES" "${CMAKE_CURRENT_SOURCE_DIR}/${hdr}")
         endif()
     endforeach()

     set("${PROJECT_NAME}_ALL_SOURCES" "${${PROJECT_NAME}_ALL_SOURCES}" CACHE STRING "" FORCE )    
     
endmacro(copy_headers_target)

