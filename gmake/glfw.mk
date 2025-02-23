# refer to main makefile for deps_ACT
glfw_ACT ?= $(deps_ACT)

# where to locate or clone the library
glfw_DIR ?= external/glfw

# where to put configuration cache
glfw_CACHEDIR = build/$(targetOS)/$(buildProfile)

# arguments: permitted actions, OS, profile, file for output
define glfw_Prepare
	lua gmake/glfw.lua $(glfw_ACT) $(targetOS) $(buildProfile) $(glfw_CACHEDIR)
endef