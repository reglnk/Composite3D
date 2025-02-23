# == what can be done to obtain libs ==
# 'clone' implies 'build'
# 'build' will run cmake and build the library if needed, according to build profile
# 'systemlib' will try to find systemwide installed library and use it
# 'provided' will try to use prebuilt library by paths provided here
# if nothing of above, only existing library will be used
glfw_ACT ?= provided,systemlib,build,clone

# where to locate or clone the library
glfw_DIR ?= external/glfw

# where to put configuration cache
glfw_CACHEDIR = build/$(targetOS)/$(buildProfile)

# arguments: permitted actions, OS, profile, file for output
define glfw_Prepare
	lua gmake/glfw.lua $(glfw_ACT) $(targetOS) $(buildProfile) $(glfw_CACHEDIR)
endef