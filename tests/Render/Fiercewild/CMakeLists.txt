set(testsRenderFiercewild_SOURCES
    "src/main.cpp"
    "${CM3D_EXTERNAL_ROOT}/imgui/imgui.cpp"
    "${CM3D_EXTERNAL_ROOT}/imgui/imgui_draw.cpp"
    "${CM3D_EXTERNAL_ROOT}/imgui/imgui_tables.cpp"
    "${CM3D_EXTERNAL_ROOT}/imgui/imgui_widgets.cpp"
    "${CM3D_EXTERNAL_ROOT}/imgui/backends/imgui_impl_glfw.cpp"
    "${CM3D_EXTERNAL_ROOT}/imgui/backends/imgui_impl_opengl3.cpp"
)
add_executable(testsRenderFiercewild ${testsRenderFiercewild_SOURCES})
add_dependencies(testsRenderFiercewild cm3dcore)
add_dependencies(testsRenderFiercewild cm3dapp)
add_dependencies(testsRenderFiercewild fiercewild)

set_target_properties(testsRenderFiercewild PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR}/tests/Render/Fiercewild
    RUNTIME_OUTPUT_NAME "render"
)
target_include_directories(testsRenderFiercewild PRIVATE "${CM3D_INTERNAL_INCDIRS}")
target_link_libraries (
testsRenderFiercewild
	"$<TARGET_FILE:cm3dcore>"
	"$<TARGET_FILE:cm3dapp>"
	"$<TARGET_FILE:fiercewild>"
	"$<TARGET_FILE:cm3dutil_gl>"
	"${CM3D_INTERNAL_ASSIMP_LIBS}"
	"${CM3D_INTERNAL_LUA_LIBS}"
	"${CM3D_INTERNAL_GLFW_LIBS}"
)
# add_custom_command (TARGET testsRenderFiercewild POST_BUILD
    # COMMAND cp -f "${CMAKE_CURRENT_SOURCE_DIR}/objects.lua" "$<TARGET_FILE_DIR:testsRenderFiercewild>"
    # COMMAND cp -rf "${CMAKE_CURRENT_SOURCE_DIR}/models" "$<TARGET_FILE_DIR:testsRenderFiercewild>"
    # COMMAND cp -rf "${CMAKE_CURRENT_SOURCE_DIR}/resources" "$<TARGET_FILE_DIR:testsRenderFiercewild>"
# )
