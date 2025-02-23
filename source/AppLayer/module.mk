
SRC_AppLayer = \
	source/AppLayer/Global.cpp \
	source/AppLayer/Platform/GLFW/Window.cpp \
	source/AppLayer/Platform/GLFW/GUI_gl3.cpp

OBJ_AppLayer := $(SRC_AppLayer:%.cpp=obj/$(targetOS)/$(buildProfile)/%.cpp.o)
OUT_AppLayer = build/$(targetOS)/$(buildProfile)/libcm3dapp.a

$(OUT_AppLayer): prepareLibs $(OBJ_AppLayer) # $(OUT_Core)
	@mkdir -p $(dir $@)
	ar rcs $@ $(OBJ_AppLayer)

obj/$(targetOS)/$(buildProfile)/source/AppLayer/%.cpp.o: source/AppLayer/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< $(INC_cm3d) $(CXXFLAGS)
