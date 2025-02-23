# == what can be done to obtain libs ==
# 'clone' implies 'build'
# 'build' will run cmake and build the library if needed, according to build profile
# 'systemlib' will try to find systemwide installed library and use it
# 'provided' will try to use prebuilt library by paths provided here
# if nothing of above, only existing library will be used
luarjit_ACT ?= provided,systemlib,build,clone

# where to locate or clone the library
luarjit_DIR ?= external/luarjit

# where to put configuration cache
luarjit_CACHEDIR = build/$(targetOS)/$(buildProfile)

# arguments: permitted actions, OS, profile, file for output
define luarjit_Prepare
	lua gmake/luarjit.lua $(luarjit_ACT) $(targetOS) $(buildProfile) $(luarjit_CACHEDIR)
endef