cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(zstd_hdf5)

FetchContent_Declare(
  libhdf5-wasm
  URL https://github.com/usnistgov/libhdf5-wasm/releases/download/v0.3.0_3.1.28/libhdf5-1_12_2-wasm.tar.gz 
  URL_HASH SHA256=7089f9bf29dc3759d7aa77848cfa12d546eabd152d40dd00a90aace99c056600
)
FetchContent_MakeAvailable(libhdf5-wasm)

FetchContent_Declare(
  zstd
  GIT_REPOSITORY https://github.com/facebook/zstd
  SOURCE_SUBDIR  build/cmake
  GIT_TAG v1.5.5
)

set(ZSTD_BUILD_TESTS OFF CACHE INTERNAL "")
set(ZSTD_MULTITHREAD_SUPPORT OFF CACHE INTERNAL "")
set(ZSTD_BUILD_PROGRAMS OFF CACHE INTERNAL "")
set(ZSTD_BUILD_TESTS OFF CACHE INTERNAL "")
set(ZSTD_BUILD_SHARED OFF CACHE INTERNAL "")
set(ZSTD_BUILD_STATIC ON CACHE INTERNAL "")
# Using this combination of GetProperties, Populate and add_subdirectory
# in order to add EXCLUDE_FROM_ALL keyword, so that zstd libs are not 
# added to installation during 'make install'
FetchContent_GetProperties(zstd)
if(NOT zstd_POPULATED)
  FetchContent_Populate(zstd)
  add_subdirectory(${zstd_SOURCE_DIR}/build/cmake ${zstd_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# find_package(zstd)

# sources
set(PLUGIN_SOURCES zstd_h5plugin.c)

# HDF5 plugin as static library
add_library(zstd_h5_plugin STATIC ${PLUGIN_SOURCES})
target_include_directories(zstd_h5_plugin PRIVATE "${zstd_SOURCE_DIR}/lib")
set_target_properties(zstd_h5_plugin PROPERTIES
    OUTPUT_NAME H5Zzstd
    POSITION_INDEPENDENT_CODE ON
)

# add_dependencies(zstd_h5_plugin libzstd_static)
target_link_libraries(zstd_h5_plugin hdf5-wasm libzstd_static)

# create combined library (including libzstd)
set(OUTPUT_FILE libH5Zzstd.so CACHE INTERNAL "Output file name")
set(PLUGIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dist CACHE PATH "")
add_custom_target(zstd_h5_plugin_shared ALL 
  COMMAND 
    ${CMAKE_C_COMPILER} -s SIDE_MODULE=1 libH5Zzstd.a ${zstd_BINARY_DIR}/lib/libzstd.a -o ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE}
  DEPENDS zstd_h5_plugin libzstd_static
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE} DESTINATION ${PLUGIN_DIR})