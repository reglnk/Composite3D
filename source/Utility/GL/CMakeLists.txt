
set (
cm3dutil_gl_SOURCES
	Preprocessor.cpp
	ShaderProgram.cpp
	"${CM3D_EXTERNAL_ROOT}/glad/src/gl.c"
)
add_library(cm3dutil_gl STATIC ${cm3dutil_gl_SOURCES})
set_target_properties(cm3dutil_gl PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR})

target_include_directories(cm3dutil_gl PRIVATE ${CM3D_INTERNAL_INCDIRS})
