add_executable(testsCoreFileSystem src/main.cpp)
add_dependencies(testsCoreFileSystem cm3dcore)

set_target_properties(testsCoreFileSystem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Core/FileSystem
    RUNTIME_OUTPUT_NAME "listfs"
)
target_include_directories(testsCoreFileSystem PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsCoreFileSystem "$<TARGET_FILE:cm3dcore>")
