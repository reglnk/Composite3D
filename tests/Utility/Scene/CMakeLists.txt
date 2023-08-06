
set_source_files_properties(src/main.cpp PROPERTIES LANGUAGE CXX)
add_executable(testsUtilityScene src/main.cpp)
add_dependencies(testsUtilityScene cm3d)

set_target_properties(testsUtilityScene PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Utility/Scene
    RUNTIME_OUTPUT_NAME "list"
)
target_include_directories(testsUtilityScene PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsUtilityScene "$<TARGET_FILE:cm3d>" "${CM3D_INTERNAL_LUA_LIBS}")
add_custom_command(TARGET testsUtilityScene POST_BUILD
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/reality.lua" "$<TARGET_FILE_DIR:testsUtilityScene>")
