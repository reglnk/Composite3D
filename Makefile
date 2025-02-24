# GNU makefile. Designed for:
# - Linux
# - BSD
# - MinGW

buildProfile ?=	opt

CXX?=		clang++
CC?=		clang

ifeq ($(buildProfile), opt)
  CFLAGS = -O3
else ifeq ($(buildProfile), dbg)
  CFLAGS = -g
else ifeq ($(buildProfile), asan)
  CFLAGS = -g -fsanitize=address
endif

CXXFLAGS :=	${CFLAGS}
CFLAGS   +=	-std=gnu99
CXXFLAGS +=	-std=gnu++20

targetOS=	default
ifeq ($(OS), Windows_NT)
  targetOS = Windows
else
  targetOS = $(shell uname -s)
endif

# == what can be done to obtain libs ==
# 'clone' implies 'build'
# 'build' will run make/cmake/whatever and build the library if needed, according to build profile
# 'systemlib' will try to find systemwide installed library and use it
# 'provided' will try to use prebuilt library by paths provided in module.mk for each dependency
deps_ACT ?= provided,systemlib,build,clone

lua_BIN ?= lua

# usually on Linux you'll find 'lua' available from everywhere in shell,
# or specific version like 'lua5.1' or 'lua5.4'
# on FreeBSD, it's 'lua54', 'lua51' and so on, probably without 'lua'

# if it's not overridden, try different variants:
ifeq ($(lua_BIN), lua)
  ifeq (, $(shell command -v lua))
    ifneq (, $(shell command -v lua5.1))
      lua_BIN = lua5.1
    else ifneq (, $(shell command -v lua51))
      lua_BIN = lua51
    else ifneq (, $(shell command -v lua5.2))
      lua_BIN = lua5.2
    else ifneq (, $(shell command -v lua52))
      lua_BIN = lua52
    else ifneq (, $(shell command -v lua5.3))
      lua_BIN = lua5.3
    else ifneq (, $(shell command -v lua53))
      lua_BIN = lua53
    else ifneq (, $(shell command -v lua5.4))
      lua_BIN = lua5.4
    else ifneq (, $(shell command -v lua54))
      lua_BIN = lua54
    endif
  endif
endif

# do this for setting no action for all dependencies
# deps_ACT = 0

include gmake/assimp.mk
include gmake/glfw.mk
include gmake/luarjit.mk

INC_cm3d = -Iinclude -Iexternal/bitsery/include -Iexternal/glad/include -Iexternal/glfw/include -Iexternal/glm -Iexternal/imgui -Iexternal/stb -Iexternal/luarjit2/src

.PHONY: default
default: all

.PHONY: prepareAssimp
prepareAssimp:
	@echo [Assimp is being prepared]
	$(call assimp_Prepare)

.PHONY: prepareGLFW
prepareGLFW:
	@echo [GLFW is being prepared]
	$(call glfw_Prepare)

.PHONY: prepareLuarJIT
prepareLuarJIT:
	@echo [LuarJIT is being prepared]
	$(call luarjit_Prepare)

# call this before making each dependent target to ensure that
# the libs' generated data is available on all gmake threads that make them
define loadCache
	$(eval $(file < $(assimp_CACHEDIR)/assimp.cache))
	$(eval $(file < $(glfw_CACHEDIR)/glfw.cache))
	$(eval $(file < $(luarjit_CACHEDIR)/luarjit.cache))
endef

.PHONY: prepareLibs
prepareLibs: prepareAssimp prepareGLFW prepareLuarJIT

# =========

include source/module.mk
include examples/module.mk
include external/module.mk

# =========

all: $(OUT_source) $(OUT_examples)

clean:
	rm -rf obj/$(targetOS)/$(buildProfile) build/$(targetOS)/$(buildProfile)

cleanall:
	rm -rf obj build
