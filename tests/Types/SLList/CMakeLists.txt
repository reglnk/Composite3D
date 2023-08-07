add_executable(testsTypesSLList src/main.cpp)

set_target_properties(testsTypesSLList PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Types/SLList
    RUNTIME_OUTPUT_NAME "sllist"
)
target_include_directories(testsTypesSLList PRIVATE "${CM3D_INTERNAL_INCDIRS}")
