set(testsModulesRenderSurface_SOURCES
    "src/main.cpp"
)
add_executable(testsModulesRenderSurface ${testsModulesRenderSurface_SOURCES})
add_dependencies(testsModulesRenderSurface cm3dcore)

set_target_properties(testsModulesRenderSurface PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Modules/RenderSurface
    RUNTIME_OUTPUT_NAME "view"
)
target_include_directories(testsModulesRenderSurface PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries(testsModulesRenderSurface "$<TARGET_FILE:cm3dcore>" "${CM3D_INTERNAL_ASSIMP_LIBS}")
if (NOT WIN32) # @todo: check on linux
    target_link_libraries(testsModulesRenderSurface pthread)
endif()
