
set (fiercewild_SOURCES
	Fiercewild.cpp
	"Modules/RenderSurface.cpp"
	"${CM3D_EXTERNAL_ROOT}/glad/src/gl.c"
)
add_library(fiercewild STATIC ${fiercewild_SOURCES})
add_dependencies(fiercewild cm3dutil_gl)

set_target_properties(fiercewild PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR})

target_include_directories(fiercewild PRIVATE ${CM3D_INTERNAL_INCDIRS})
