# @todo

set (cm3d_SOURCES
	Core/FileSystem.cpp
	Core/FPCamera.cpp
	Core/GLShaderProgram.cpp
	Core/GLSLPreprocessor.cpp
	Core/HardInfo.cpp
	Core/LogManager.cpp
	Core/Vector3.cpp
)

set_source_files_properties(${cm3d_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(cm3d STATIC ${cm3d_SOURCES})
