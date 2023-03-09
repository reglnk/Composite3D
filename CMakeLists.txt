cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Composite3D VERSION 0.1.0 LANGUAGES CXX)

set(CM3D_RUNTIME_OUTPUT_DIR "${CMAKE_BINARY_DIR}" CACHE PATH "Put binary in this folder")
option(CM3D_BUILD_TESTS "" ON)

set(CM3D_INTERNAL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "")
set(CM3D_EXTERNAL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external" CACHE INTERNAL "")

# The include dirs used by cm3d and subprojects
set(CM3D_INTERNAL_INCDIRS "" CACHE INTERNAL "")

set(CM3D_INTERNAL_ASSIMP_LIBS "" CACHE INTERNAL "")
set(CM3D_INTERNAL_GLFW_LIBS "" CACHE INTERNAL "")
set(CM3D_INTERNAL_LUA_LIBS "lua" CACHE INTERNAL "")

list(APPEND CM3D_INTERNAL_INCDIRS
	"${CM3D_INTERNAL_ROOT}/include"
	"${CM3D_INTERNAL_ROOT}/external/glm"
	"${CM3D_INTERNAL_ROOT}/external/stb"
	"${CM3D_INTERNAL_ROOT}/external/glad/include"
	"${CM3D_INTERNAL_ROOT}/external/lua/src"
)

# Assimp configuration ================

set(EXT_ASSIMP_INCLUDE_DIR "" CACHE PATH "")
set(EXT_ASSIMP_BUILD_INCLUDE_DIR "" CACHE PATH "Contains config.h")
set(EXT_ASSIMP_LIBRARY_PATH "" CACHE FILEPATH "Import library file")

if (EXT_ASSIMP_INCLUDE_DIR AND
	EXT_ASSIMP_BUILD_INCLUDE_DIR AND
	EXT_ASSIMP_LIBRARY_PATH
)
	message(STATUS "Using Assimp with provided paths")
	list(APPEND CM3D_INTERNAL_INCDIRS
		"${EXT_ASSIMP_INCLUDE_DIR}"
		"${EXT_ASSIMP_BUILD_INCLUDE_DIR}"
	)
	set(CM3D_INTERNAL_ASSIMP_LIBS "${EXT_ASSIMP_LIBRARY_PATH}" CACHE INTERNAL "")
else()
	find_package(assimp REQUIRED)
	message(STATUS "Found assimp package")
	include_directories(SYSTEM ${ASSIMP_INCLUDE_DIRS})
	set(CM3D_INTERNAL_ASSIMP_LIBS "${ASSIMP_LIBRARIES}" CACHE INTERNAL "")
endif()

# GLFW configuration ==================

set(EXT_GLFW_INCLUDE_DIR "" CACHE PATH "")
set(EXT_GLFW_LIBRARY_PATH "" CACHE FILEPATH "Import library file")

if (EXT_GLFW_INCLUDE_DIR AND
	EXT_GLFW_LIBRARY_PATH
)
	message(STATUS "Using GLFW with provided paths")
	list(APPEND CM3D_INTERNAL_INCDIRS
		"${EXT_GLFW_INCLUDE_DIR}"
	)
	set(CM3D_INTERNAL_GLFW_LIBS "${EXT_GLFW_LIBRARY_PATH}" CACHE INTERNAL "")
else()
	find_package(glfw3 3.3 REQUIRED)
	message(STATUS "Found glfw3 package")
	# GLFW includes system directories itself
	set(CM3D_INTERNAL_GLFW_LIBS "glfw" CACHE INTERNAL "")
endif()

# =====================================

add_subdirectory(external/lua) # targets: lua
add_subdirectory(source) # targets: cm3d
add_dependencies(cm3d lua)

if (CM3D_BUILD_TESTS)
	add_subdirectory(tests)
endif()

set_target_properties(cm3d PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CM3D_RUNTIME_OUTPUT_DIR})

target_include_directories(cm3d PRIVATE ${CM3D_INTERNAL_INCDIRS})
target_link_libraries(cm3d PRIVATE
	${CM3D_INTERNAL_ASSIMP_LIBS}
	${CM3D_INTERNAL_GLFW_LIBS}
	${CM3D_INTERNAL_LUA_LIBS}
)
