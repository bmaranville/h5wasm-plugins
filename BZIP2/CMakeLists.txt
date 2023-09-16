cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(bz2_h5wasm)

FetchContent_Declare(
  libhdf5-wasm
  URL https://github.com/usnistgov/libhdf5-wasm/releases/download/v0.3.0_3.1.28/libhdf5-1_12_2-wasm.tar.gz 
  URL_HASH SHA256=7089f9bf29dc3759d7aa77848cfa12d546eabd152d40dd00a90aace99c056600
)
FetchContent_MakeAvailable(libhdf5-wasm)

FetchContent_Declare(
  bzip2
  # GIT_REPOSITORY https://github.com/libarchive/bzip2
  GIT_REPOSITORY https://gitlab.com/bzip2/bzip2
  GIT_TAG 66c46b8c9436613fd81bc5d03f63a61933a4dcc3
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(ENABLE_STATIC_LIB ON CACHE INTERNAL "")
set(ENABLE_LIB_ONLY ON CACHE INTERNAL "")

FetchContent_GetProperties(bzip2)
if(NOT bzip2_POPULATED)
  FetchContent_Populate(bzip2)
  add_subdirectory(${bzip2_SOURCE_DIR} ${bzip2_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# sources
file(GLOB PLUGIN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
# set(PLUGIN_SOURCES H5Zlz4.c lz4_h5plugin.c)

# HDF5 plugin as static library
message(STATUS ${PLUGIN_SOURCES})
add_library(bz2_h5_plugin STATIC ${PLUGIN_SOURCES})
target_compile_definitions(bz2_h5_plugin PUBLIC HAVE_BZ2_LIB)
target_include_directories(bz2_h5_plugin PUBLIC ${bzip2_SOURCE_DIR})
set_target_properties(bz2_h5_plugin PROPERTIES
    OUTPUT_NAME H5Zbz2
    POSITION_INDEPENDENT_CODE ON
)

# add_dependencies(lz4_h5_plugin libzstd_static)
target_link_libraries(bz2_h5_plugin hdf5-wasm bz2_static)
message(STATUS "prefix: ${CMAKE_INSTALL_PREFIX}")

# create combined library (including liblz4)
set(OUTPUT_FILE libH5Zbz2.so CACHE INTERNAL "Output file name")
add_custom_target(bz2_h5_plugin_shared ALL 
  COMMAND 
    ${CMAKE_C_COMPILER} -s SIDE_MODULE=1 libH5Zbz2.a ${bzip2_BINARY_DIR}/libbz2_static.a -o ${OUTPUT_FILE}
  DEPENDS bz2_h5_plugin ${bzip2_LIBRARIES}
)

set(PLUGIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../dist CACHE PATH "")
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_FILE} DESTINATION ${PLUGIN_DIR})
