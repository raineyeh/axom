################################
# Conduit
################################
project(conduit)

include(${PROJECT_SOURCE_DIR}/src/CMake/BasicChecks.cmake)

file(MAKE_DIRECTORY ${HEADER_INCLUDES_DIRECTORY}/conduit)
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/src/conduit/Conduit_Config.h.in"
                "${HEADER_INCLUDES_DIRECTORY}/conduit/Conduit_Config.h" )

#
# Specify all headers
#
set(conduit_headers
    ${PROJECT_SOURCE_DIR}/src/conduit/Bitwidth_Style_Types.h
    ${PROJECT_SOURCE_DIR}/src/conduit/DataArray.h
    ${PROJECT_SOURCE_DIR}/src/conduit/Endianness.h
    ${PROJECT_SOURCE_DIR}/src/conduit/Node.h
    ${PROJECT_SOURCE_DIR}/src/conduit/conduit.h
    ${PROJECT_SOURCE_DIR}/src/conduit/Core.h
    ${PROJECT_SOURCE_DIR}/src/conduit/DataType.h
    ${PROJECT_SOURCE_DIR}/src/conduit/Generator.h
    ${PROJECT_SOURCE_DIR}/src/conduit/Error.h
    ${PROJECT_SOURCE_DIR}/src/conduit/NodeIterator.h
    ${PROJECT_SOURCE_DIR}/src/conduit/Schema.h
    ${PROJECT_SOURCE_DIR}/src/conduit/Utils.h
    ${PROJECT_SOURCE_DIR}/src/conduit/Conduit_Exports.h
    )

#
# Specify all sources
#
set(conduit_sources
    src/conduit/Core.cpp
    src/conduit/Error.cpp
    src/conduit/Endianness.cpp
    src/conduit/DataType.cpp
    src/conduit/DataArray.cpp
    src/conduit/Generator.cpp
    src/conduit/Node.cpp
    src/conduit/NodeIterator.cpp
    src/conduit/Schema.cpp
    src/conduit/Utils.cpp
    )

make_library(conduit "${conduit_sources}" "${conduit_headers}")
add_dependencies(conduit copy_headers_conduit)
target_include_directories(conduit PRIVATE
    ${PROJECT_SOURCE_DIR}/src/thirdparty_builtin/rapidjson/include)
    
target_include_directories(conduit PRIVATE
                           ${HEADER_INCLUDES_DIRECTORY}/conduit/)



install(TARGETS conduit DESTINATION lib EXPORT ${PROJECT_NAME}-targets)


#
# custom target to copy header files in the "include" folder of the build tree.
#
copy_headers_target( ${PROJECT_NAME} "${conduit_headers}"
                     ${HEADER_INCLUDES_DIRECTORY}/conduit/ )

################################
# Create CMake importable
# exports for all of our targets
################################
install(EXPORT ${PROJECT_NAME}-targets DESTINATION lib/cmake)


