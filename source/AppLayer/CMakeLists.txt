
set (
cm3dapp_SOURCES
	Global.cpp
	Platform/GLFW/Window.cpp
	Platform/GLFW/GUI_gl3.cpp
)
add_library(cm3dapp STATIC ${cm3dapp_SOURCES})
add_dependencies(cm3dapp cm3dcore)
set_target_properties(cm3dapp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR})

target_include_directories(cm3dapp PRIVATE ${CM3D_INTERNAL_INCDIRS})
target_link_libraries(cm3dapp PRIVATE ${CM3D_INTERNAL_GLFW_LIBS})
