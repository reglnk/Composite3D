add_executable(testsTypesDynArrayPerfOwn perf/dynarr.cpp)
add_executable(testsTypesDynArrayPerfSTL perf/stdvec.cpp)

set_target_properties(testsTypesDynArrayPerfOwn PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Types/DynArrayPerf
    RUNTIME_OUTPUT_NAME "dynarr"
)
set_target_properties(testsTypesDynArrayPerfSTL PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Types/DynArrayPerf
    RUNTIME_OUTPUT_NAME "stdvec"
)
target_include_directories(testsTypesDynArrayPerfOwn PRIVATE "${CM3D_INTERNAL_INCDIRS}")
