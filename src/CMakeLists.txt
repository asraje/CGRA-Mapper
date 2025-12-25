#
# ======================================================================
# CMakeLists.txt
# ======================================================================
# CMakeLists for mapper pass.
#
# Author : Cheng Tan
#  Date : July 16, 2019
#

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

# Link against OpenMP
target_link_libraries(mapperPass PRIVATE OpenMP::OpenMP_CXX)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(mapperPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked).
# For pass plugins, we do NOT link against LLVM libraries directly -
# the symbols are provided by the host tool (opt, clang, etc.)
if(APPLE)
    set_target_properties(mapperPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
else()
    # On Linux, allow unresolved symbols - they will be resolved at runtime
    # when the plugin is loaded into opt/clang
    set_target_properties(mapperPass PROPERTIES
        LINK_FLAGS "-Wl,--unresolved-symbols=ignore-in-object-files"
    )
endif()
