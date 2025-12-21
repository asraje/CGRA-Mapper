#
# ======================================================================
# CMakeLists.txt
# ======================================================================
# CMakeLists for mapper pass.
#
# Author : Cheng Tan
#  Date : July 16, 2019
#
cmake_minimum_required(VERSION 3.20)
project(mapperPass LANGUAGES C CXX)
if(APPLE)
  set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
  set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
  set(OpenMP_C_LIB_NAMES "omp")
  set(OpenMP_CXX_LIB_NAMES "omp")
  set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")
  include_directories("/opt/homebrew/opt/libomp/include")
endif()

find_package(OpenMP REQUIRED)
add_library(mapperPass MODULE
    # List your source files here.
    mapperPass.cpp
    DFGNode.h
    DFGNode.cpp
    DFGEdge.h
    DFGEdge.cpp
    DFG.h
    DFG.cpp
    CGRANode.h
    CGRANode.cpp
    CGRALink.h
    CGRALink.cpp
    CGRA.h
    CGRA.cpp
    Mapper.h
    Mapper.cpp
)

target_link_libraries(mapperPass PRIVATE OpenMP::OpenMP_CXX)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
#if (NOT CMAKE_VERSION VERSION_LESS 3.1)
#  target_compile_features(mapperPass PRIVATE cxx_range_for cxx_auto_type)
#endif ()

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(mapperPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(mapperPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)
