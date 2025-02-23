
SRW_RenderFiercewild = Fiercewild.cpp Modules/RenderSurface.cpp
SRC_RenderFiercewild := $(SRW_RenderFiercewild:%=source/Render/Fiercewild/%)
OBJ_RenderFiercewild := $(SRC_RenderFiercewild:%.cpp=obj/$(targetOS)/$(buildProfile)/%.cpp.o)
OBJ_RenderFiercewild := $(OBJ_RenderFiercewild:%.c=obj/$(targetOS)/$(buildProfile)/%.c.o)
OUT_RenderFiercewild = build/$(targetOS)/$(buildProfile)/libfiercewild.a

$(OUT_RenderFiercewild): prepareLibs $(OBJ_RenderFiercewild)
	ar rcs $@ $(OBJ_RenderFiercewild)

obj/$(targetOS)/$(buildProfile)/source/Render/Fiercewild/%.cpp.o: source/Render/Fiercewild/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(INC_cm3d) $(CXXFLAGS)