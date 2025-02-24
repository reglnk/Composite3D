# refer to main makefile for deps_ACT
luarjit_ACT ?= $(deps_ACT)

# where to locate or clone the library
luarjit_DIR ?= external/luarjit

# where to put configuration cache
luarjit_CACHEDIR = build/$(targetOS)/$(buildProfile)

# arguments: permitted actions, OS, profile, file for output
define luarjit_Prepare
	$(lua_BIN) gmake/luarjit.lua $(luarjit_ACT) $(targetOS) $(buildProfile) $(luarjit_CACHEDIR)
endef
