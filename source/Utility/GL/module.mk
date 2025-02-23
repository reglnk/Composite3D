
SRC_UtilityGL = \
	source/Utility/GL/Preprocessor.cpp \
	source/Utility/GL/ShaderProgram.cpp \
	external/glad/src/gl.c

OBJ_UtilityGL := $(SRC_UtilityGL:source/Utility/GL/%.cpp=obj/$(targetOS)/$(buildProfile)/source/Utility/GL/%.cpp.o)
OBJ_UtilityGL := $(OBJ_UtilityGL:external/glad/src/gl.c=obj/$(targetOS)/$(buildProfile)/source/Utility/GL/glad_gl.c.o)

OUT_UtilityGL = build/$(targetOS)/$(buildProfile)/libcm3dutil_gl.a

$(OUT_UtilityGL): prepareLibs $(OBJ_UtilityGL) $(OUT_Core) $(OUT_RenderFiercvewild)
	@mkdir -p $(dir $@)
	ar rcs $@ $(OBJ_UtilityGL)

obj/$(targetOS)/$(buildProfile)/source/Utility/GL/%.cpp.o: source/Utility/GL/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(INC_cm3d) $(CXXFLAGS)

obj/$(targetOS)/$(buildProfile)/source/Utility/GL/glad_gl.c.o: external/glad/src/gl.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(INC_cm3d) $(CFLAGS)
