################################
# Conduit
################################
project(conduit)

include(${PROJECT_SOURCE_DIR}/src/CMake/BasicChecks.cmake)

file(MAKE_DIRECTORY ${HEADER_INCLUDES_DIRECTORY}/conduit)
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/src/conduit/Conduit_Config.hpp.in"
                "${HEADER_INCLUDES_DIRECTORY}/conduit/Conduit_Config.hpp" )

#
# Specify all headers
#
set(conduit_headers
    ${PROJECT_SOURCE_DIR}/src/conduit/Bitwidth_Style_Types.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/DataArray.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Endianness.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Node.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/conduit.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Core.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/DataType.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Generator.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Error.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/NodeIterator.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Schema.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Utils.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Conduit_Exports.hpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Conduit_License.hpp
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

make_library(LIBRARY_NAME conduit LIBRARY_SOURCES "${conduit_sources}")
target_include_directories(conduit PRIVATE ${HEADER_INCLUDES_DIRECTORY}/conduit)
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


