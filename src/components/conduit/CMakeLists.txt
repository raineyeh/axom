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

foreach(hdr ${conduit_headers})
 message(STATUS "copy ${hdr} to ${HEADER_INCLUDES_DIRECTORY}/conduit")
 file(COPY ${hdr} DESTINATION ${HEADER_INCLUDES_DIRECTORY}/conduit)
endforeach()

#
# Specify all sources
#
set(conduit_sources
    ${PROJECT_SOURCE_DIR}/src/conduit/Core.cpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Error.cpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Endianness.cpp
    ${PROJECT_SOURCE_DIR}/src/conduit/DataType.cpp
    ${PROJECT_SOURCE_DIR}/src/conduit/DataArray.cpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Generator.cpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Node.cpp
    ${PROJECT_SOURCE_DIR}/src/conduit/NodeIterator.cpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Schema.cpp
    ${PROJECT_SOURCE_DIR}/src/conduit/Utils.cpp
    )

make_library(conduit "${conduit_sources}")
target_include_directories(conduit PRIVATE ${HEADER_INCLUDES_DIRECTORY}/conduit)
target_include_directories(conduit PRIVATE
    ${PROJECT_SOURCE_DIR}/src/thirdparty_builtin/rapidjson/include)

install(TARGETS conduit DESTINATION lib EXPORT ${PROJECT_NAME}-targets)

################################
# Create CMake importable
# exports for all of our targets
################################
install(EXPORT ${PROJECT_NAME}-targets DESTINATION lib/cmake)