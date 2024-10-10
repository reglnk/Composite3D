add_executable(testsSerialization src/main.cpp)
add_dependencies(testsSerialization cm3dcore)

set_target_properties(testsSerialization PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Serialization
    RUNTIME_OUTPUT_NAME "serial"
)
target_include_directories(testsSerialization PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsSerialization "$<TARGET_FILE:cm3dcore>")
