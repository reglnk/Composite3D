# == what can be done to obtain libs ==
# 'clone' implies 'build'
# 'build' will run cmake and build the library if needed, according to build profile
# 'systemlib' will try to find systemwide installed library and use it
# 'provided' will try to use prebuilt library by paths provided here
# if nothing of above, only existing library will be used
assimp_ACT ?= provided,systemlib,build,clone

# where to locate or clone the library
assimp_DIR ?= external/assimp

# where to put configuration cache
assimp_CACHEDIR = build/$(targetOS)/$(buildProfile)

# paths relative to assimp_DIR
# assimp_INC = $(assimp_DIR)/include
# assimp_BDIR = $(assimp_DIR)/build/$(targetOS)/$(buildProfile)
# assimp_LIB = $(assimp_BDIR)/lib/libassimp.dll.a:$(assimp_BDIR)/lib/libassimp.dll.a

# arguments: permitted actions, OS, profile, file for output
define assimp_Prepare
	lua gmake/assimp.lua $(assimp_ACT) $(targetOS) $(buildProfile) $(assimp_CACHEDIR)
endef