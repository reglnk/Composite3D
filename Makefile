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
ifeq ($(OS),Windows_NT)
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
	$(call assimp_Prepare)

.PHONY: prepareGLFW
prepareGLFW:
	$(call glfw_Prepare)

.PHONY: prepareLuarJIT
prepareLuarJIT:
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
