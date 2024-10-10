add_executable(testsTypesUniqueStorage src/main.cpp)

set_target_properties(testsTypesUniqueStorage PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Types/UniqueStorage
    RUNTIME_OUTPUT_NAME "test"
)
target_include_directories(testsTypesUniqueStorage PRIVATE "${CM3D_INTERNAL_INCDIRS}")
