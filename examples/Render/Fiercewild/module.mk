
SRC_examplesRenderFiercewild = \
	examples/Render/Fiercewild/src/main.cpp \
    external/imgui/imgui.cpp \
    external/imgui/imgui_draw.cpp \
    external/imgui/imgui_tables.cpp \
    external/imgui/imgui_widgets.cpp \
    external/imgui/backends/imgui_impl_glfw.cpp \
    external/imgui/backends/imgui_impl_opengl3.cpp

OBJ_examplesRenderFiercewild := $(SRC_examplesRenderFiercewild:examples/%.cpp=obj/$(targetOS)/$(buildProfile)/examples/%.cpp.o)

# place imgui objects here to not conflict with other possible targets which require ImGui
OBJ_examplesRenderFiercewild := $(OBJ_examplesRenderFiercewild:external/imgui/%.cpp=obj/$(targetOS)/$(buildProfile)/examples/Render/Fiercewild/imgui/%.cpp.o)

OUTDIR_examplesRenderFiercewild = build/$(targetOS)/$(buildProfile)/examples/fiercewild
OUT_examplesRenderFiercewild = $(OUTDIR_examplesRenderFiercewild)/render

$(OUT_examplesRenderFiercewild): prepareLibs \
	$(OBJ_examplesRenderFiercewild) \
	$(OUT_Core) \
	$(OUT_RenderFiercvewild) \
	$(OUT_UtilityGL) \
	$(OUT_AppLayer) \
	
	$(call loadCache)
	@mkdir -p $(dir $@)
	$(CXX) -o $@ $(OBJ_examplesRenderFiercewild) $(OUT_Core) $(OUT_RenderFiercewild) $(OUT_UtilityGL) $(OUT_AppLayer) $(glfw_LIB) $(assimp_LIB) $(luarjit_LIB)
	@cp -rnv examples/Render/Fiercewild/models $(OUTDIR_examplesRenderFiercewild)
	@cp -rnv examples/Render/Fiercewild/resources $(OUTDIR_examplesRenderFiercewild)
	@cp -nv examples/Render/Fiercewild/objects.lua $(OUTDIR_examplesRenderFiercewild)

obj/$(targetOS)/$(buildProfile)/examples/Render/Fiercewild/%.cpp.o: examples/Render/Fiercewild/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(INC_cm3d) $(CXXFLAGS)

obj/$(targetOS)/$(buildProfile)/examples/Render/Fiercewild/imgui/%.cpp.o: external/imgui/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(INC_cm3d) $(CXXFLAGS)
