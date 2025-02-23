
SRW_Core = \
	FileSystem.cpp \
	FPCamera.cpp \
	GlobalState.cpp \
	Graphics.cpp \
	HardInfo.cpp \
	LogManager.cpp \
	Vector3.cpp \
	World.cpp

SRC_Core := $(SRW_Core:%=source/Core/%)
OBJ_Core := $(SRC_Core:%.cpp=obj/$(targetOS)/$(buildProfile)/%.cpp.o)
OBJ_Core := $(OBJ_Core:%.c=obj/$(targetOS)/$(buildProfile)/%.c.o)
OUT_Core = build/$(targetOS)/$(buildProfile)/libcm3dcore.a

$(OUT_Core): prepareLibs $(OBJ_Core)
	ar rcs $@ $(OBJ_Core)

obj/$(targetOS)/$(buildProfile)/source/Core/%.cpp.o: source/Core/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(INC_cm3d) $(CXXFLAGS)