# The main required library

set (cm3dcore_SOURCES
	FileSystem.cpp
	FPCamera.cpp
	GlobalState.cpp
	Graphics.cpp
	HardInfo.cpp
	LogManager.cpp
	Vector3.cpp
)

set_source_files_properties(${cm3dcore_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(cm3dcore STATIC ${cm3dcore_SOURCES})
add_dependencies(cm3dcore lua)

set_target_properties(cm3dcore PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR})

target_include_directories(cm3dcore PRIVATE ${CM3D_INTERNAL_INCDIRS})
target_link_libraries(cm3dcore PRIVATE
	${CM3D_INTERNAL_ASSIMP_LIBS}
	${CM3D_INTERNAL_GLFW_LIBS}
	${CM3D_INTERNAL_LUA_LIBS}
)
