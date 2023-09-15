cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(lz4_h5wasm)

FetchContent_Declare(
  libhdf5-wasm
  URL https://github.com/usnistgov/libhdf5-wasm/releases/download/v0.3.0_3.1.28/libhdf5-1_12_2-wasm.tar.gz 
  URL_HASH SHA256=7089f9bf29dc3759d7aa77848cfa12d546eabd152d40dd00a90aace99c056600
)
FetchContent_MakeAvailable(libhdf5-wasm)

FetchContent_Declare(
  lz4
  GIT_REPOSITORY https://github.com/lz4/lz4
  SOURCE_SUBDIR  build/cmake
  GIT_TAG v1.9.4
)

set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
set(LZ4_BUILD_CLI OFF CACHE INTERNAL "")
set(LZ4_BUILD_LEGACY_LZ4C OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(lz4)

# sources
file(GLOB PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
# set(PLUGIN_SOURCES H5Zlz4.c lz4_h5plugin.c)

# HDF5 plugin as static library
message(STATUS ${PLUGIN_SOURCES})
add_library(lz4_h5_plugin STATIC ${PLUGIN_SOURCES})
set_target_properties(lz4_h5_plugin PROPERTIES
    OUTPUT_NAME H5Zlz4
    POSITION_INDEPENDENT_CODE ON
)

# add_dependencies(lz4_h5_plugin libzstd_static)
target_link_libraries(lz4_h5_plugin hdf5-wasm lz4_static)
message(STATUS "prefix: ${CMAKE_INSTALL_PREFIX}")

# create combined library (including liblz4)
set(OUTPUT_FILE libH5Zlz4.so CACHE INTERNAL "Output file name")
add_custom_target(lz4_h5_plugin_shared ALL 
  COMMAND 
    ${CMAKE_C_COMPILER} -s SIDE_MODULE=1 libH5Zlz4.a ${lz4_BINARY_DIR}/liblz4.a -o ${OUTPUT_FILE}
  DEPENDS lz4_h5_plugin lz4_static
)

set(PLUGIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dist CACHE PATH "")
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE} DESTINATION ${PLUGIN_DIR})
