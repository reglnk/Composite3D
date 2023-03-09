
set(testsModulesRenderSurface_SOURCES
    "src/main.cpp"
)
set_source_files_properties(${testsModulesRenderSurface_SOURCES} PROPERTIES LANGUAGE CXX)
add_executable(testsModulesRenderSurface src/main.cpp)
add_dependencies(testsModulesRenderSurface cm3d)

set_target_properties(testsModulesRenderSurface PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Modules/RenderSurface
    RUNTIME_OUTPUT_NAME "view"
)
target_include_directories(testsModulesRenderSurface PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsModulesRenderSurface "$<TARGET_FILE:cm3d>" "${CM3D_INTERNAL_ASSIMP_LIBS}")
