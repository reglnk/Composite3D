add_executable(testsCoreMath src/main.cpp)
add_dependencies(testsCoreMath cm3dcore)

set_target_properties(testsCoreMath PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Core/Math
    RUNTIME_OUTPUT_NAME "math"
)
target_include_directories(testsCoreMath PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsCoreMath "$<TARGET_FILE:cm3dcore>")
