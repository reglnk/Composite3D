
set_source_files_properties(src/main.cpp PROPERTIES LANGUAGE CXX)
add_executable(testsUtilityScene src/main.cpp)

set_target_properties(testsUtilityScene PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Utility/Scene
    RUNTIME_OUTPUT_NAME "list"
)
target_include_directories(testsUtilityScene PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsUtilityScene "${CM3D_INTERNAL_LUA_LIBS}")
