add_executable(testsTypesQueue src/main.cpp)

set_target_properties(testsTypesQueue PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Types/Queue
    RUNTIME_OUTPUT_NAME "queue"
)
target_include_directories(testsTypesQueue PRIVATE "${CM3D_INTERNAL_INCDIRS}")
