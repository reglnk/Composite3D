# refer to main makefile for deps_ACT
assimp_ACT ?= $(deps_ACT)

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
	$(lua_BIN) gmake/assimp.lua $(assimp_ACT) $(targetOS) $(buildProfile) $(assimp_CACHEDIR)
endef
