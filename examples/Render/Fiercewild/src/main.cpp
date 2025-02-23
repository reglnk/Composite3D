#include <cm3d/Core/World.hpp>
#include <cm3d/Core/FileSystem.hpp>
#include <cm3d/Core/FPCamera.hpp>
#include <cm3d/Core/GlobalState.hpp>

#include <cm3d/Modules/RenderSurface.hpp>
#include <cm3d/Modules/Position.hpp>
#include <cm3d/Render/Fiercewild/Fiercewild.hpp>

#include <cm3d/AppLayer/Global.hpp>
#include <cm3d/AppLayer/Window.hpp>
#include <cm3d/AppLayer/GUI.hpp>

#include <lua.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <unordered_map>
#include <iostream>
#include <memory>

// @todo NRPhysics
// @todo world saving

using namespace cm3d;
using Render::Fiercewild::Fiercewild;

struct userData
{
	Render::Base *renderer;
	FPCamera *camera;
	int wWidth;
	int wHeight;

	// applies single time after capturing mouse pointer
	bool cursorEntry = true;

	double prevCursorX;
	double prevCursorY;

	bool activeGUI = true;
};

int main(int argc, char **argv)
{
	using FileSystem::checkNode;
	using FileSystem::NodeType;
	using FileSystem::NodeState;

	int retcode;
	NodeState st;

	const char *objFile = argc > 1 ? argv[1] : "objects.lua";
	if (checkNode(objFile, &st) != 0) {
		std::cerr << objFile << " not found\n";
		return 1;
	}

	lua_State* L = luaL_newstate();
	luaopen_base(L);
	lua_settop(L, 0);

	retcode = luaL_dofile(L, objFile);
	if (retcode) {
		std::cerr << "Failed to execute " << objFile << '\n';
		lua_close(L);
		return retcode;
	}

	lua_getglobal(L, "objects");
	if (!lua_istable(L, 1)) {
		std::cerr << "'objects' isn't existing global table\n";
		lua_close(L);
		return 1;
	}

	size_t tsize = lua_objlen(L, 1);
	std::cout << "objects: " << tsize << '\n';

	std::unordered_map<FileSystem::sPath, std::shared_ptr<Modules::RenderSurface>> rsModels;
	World world;

	LogManager logmgr("latest.log");

	for (size_t n = 1; n <= tsize; ++n)
	{
		lua_rawgeti(L, 1, n);
		if (!lua_istable(L, 2)) {
			std::cerr << n << ": isn't a table\n";
			lua_close(L);
			return 1;
		}

		lua_pushstring(L, "path");
		lua_rawget(L, 2);
		if (!lua_isstring(L, 3)) {
			std::cerr << n << ": 'path' key is missing\n";
			lua_close(L);
			return 1;
		}
		FileSystem::sPath path = lua_tostring(L, 3);
		lua_settop(L, 2);

		lua_pushstring(L, "position");
		lua_rawget(L, 2);
		if (!lua_istable(L, 3)) {
			std::cerr << n << ": 'position' key is missing\n";
			lua_close(L);
			return 1;
		}
		lua_rawgeti(L, 3, 1);
		lua_rawgeti(L, 3, 2);
		lua_rawgeti(L, 3, 3);
		if (!(lua_isnumber(L, 4) && lua_isnumber(L, 5) && lua_isnumber(L, 6))) {
			std::cerr << n << ": position is ill-formed\n";
			lua_close(L);
			return 1;
		}
		auto m = new Modules::Position();
		m->value.x = lua_tonumber(L, 4);
		m->value.y = lua_tonumber(L, 5);
		m->value.z = lua_tonumber(L, 6);
		lua_settop(L, 2);

		if (!rsModels.count(path))
		{
			auto rs = std::make_shared<Modules::RenderSurface>();
			String outp;
			int loadst = rs->load(path, &outp);
			if (loadst)
			{
				logmgr.push(outp);
				lua_close(L);
				delete m;
				return 1;
			}
			rsModels[path] = rs;
		}
		Object obj;
		obj.addModule(m);
		obj.addShared(rsModels[path]);

		world.objects.push(std::move(obj));
		lua_settop(L, 1);
	}
	lua_close(L);

	app::winSystem();

	auto udata = std::make_unique<userData>();
	Fiercewild render("./resources/shaders/fiercewild", &logmgr);

	udata->wWidth = 800;
	udata->wHeight = 600;

	render.preconfigure();
	app::Window wnd(udata->wWidth, udata->wHeight, "Fiercewild");

	// ===========================================================

	wnd.makeCurrent();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls

	app::gui::init(wnd);
	gs::addShutdownTask([](){ImGui::DestroyContext();});

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// ===========================================================

	// @todo unvirtualize methods of renderer
	render.bindWindow((GLFWwindow *)wnd.getHandle());
	render.initialize(&retcode);
	if (retcode)
	{
		logmgr.push("Failed to initialize renderer\n");
		return 1;
	}

	auto cp = glm::vec3(5.0, 2.5, -2.0);
	if (argc > 4)
	{
		cp.x = atof(argv[2]);
		cp.y = atof(argv[3]);
		cp.z = atof(argv[4]);
	}

	FPCamera cam(cp);
	udata->renderer = &render;
	udata->camera = &cam;

	wnd.addKeyCallback([&udata](app::keyboardEvent &ev) {
		if (ImGui::GetIO().WantCaptureKeyboard)
			return;

		// std::cout << ev.key << '\n';
		auto window = reinterpret_cast<GLFWwindow *>(ev.window->getHandle());

		if (ev.action != app::Action::Release)
		{
			if (ev.key == GLFW_KEY_ESCAPE) {
				if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					udata->activeGUI = true;
				}
			}
		}
	});

	wnd.addResizeCallback([&udata](app::resizeEvent &ev) {
		udata->renderer->syncFramebuffer(ev.width, ev.height);
		udata->wWidth = ev.width;
		udata->wHeight = ev.height;
	});

	wnd.addMouseCallback([&udata](app::mouseEvent &ev)
	{
		if (ImGui::GetIO().WantCaptureMouse)
			return;

		auto window = (GLFWwindow *)ev.window->getHandle();
		if (ev.button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (ev.action != app::Action::Release)
			{
				if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					udata->cursorEntry = true;
					udata->activeGUI = false;
				}
			}
		}
	});
	wnd.addCursorOffsetCallback([&udata](app::cursorOffsetEvent &ev)
	{
		auto window = (GLFWwindow *)ev.window->getHandle();
		if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
			return;

		auto &u = udata;
		if (u->cursorEntry)
		{
			u->cursorEntry = false;
			u->prevCursorX = ev.x;
			u->prevCursorY = ev.y;
			return;
		}
		u->camera->offsetCallback(ev.x - u->prevCursorX, ev.y - u->prevCursorY);
		u->prevCursorX = ev.x;
		u->prevCursorY = ev.y;

		auto &cam = *u->camera;
		// std::cout << "camera dir: (" << (double)cam.getForward().x << ", " << (double)cam.getForward().y << ", " << (double)cam.getForward().z << ")\n";
	});
	glfwSwapInterval(1);

	render.bindWorld(world);
	for (auto iter = world.objects.begin(); iter != world.objects.end(); ++iter)
		render.loadObject(world, ~iter);

	render.syncFramebuffer(udata->wWidth, udata->wHeight);
	double prevT = app::getTime();
	while (!wnd.shouldClose())
	{
		app::gui::newFrame();
		ImGui::NewFrame();

		app::pollEvents();

		using md = FPCamera::MovDirection;
		int cmov = 0;

		if (!ImGui::GetIO().WantCaptureKeyboard)
		{
			if (wnd.getKey(GLFW_KEY_W))
				cmov = md::Forward;
			else if (wnd.getKey(GLFW_KEY_S))
				cmov = md::Backward;
			if (wnd.getKey(GLFW_KEY_A))
				cmov |= md::Left;
			else if (wnd.getKey(GLFW_KEY_D))
				cmov |= md::Right;
			if (wnd.getKey(GLFW_KEY_SPACE))
				cmov |= md::absUp;
			else if (wnd.getKey(GLFW_KEY_LEFT_SHIFT))
				cmov |= md::absDown;
		}

		double curT = app::getTime();
		double dT = 0.017; //curT - prevT;
		prevT = curT;

		cam.applyVelDelta(cmov, dT);
		cam.processPhysics(dT);
		// static unsigned char c = 0;
		// c++;
		// if (!c) {
		// 	std::cout << "camera pos: (" << (double)cam.wPos.x << ", " << (double)cam.wPos.y << ", " << (double)cam.wPos.z << ")\n";
		// }
		glm::mat4 viewMat = cam.viewMatrixRelative((float)udata->wWidth / (float)udata->wHeight);

		if (udata->activeGUI)
		{
			static float fx = 0.0f;
			static float fy = 0.0f;
			static float fz = 0.0f;
			static int counter = 0;
			static char buf[1024];

			ImGui::Begin("Hello, world!");

			ImGui::Text("This is some useful text.");
			ImGui::InputText("text input", buf, 1024);

			if (ImGui::SliderFloat("x", &fx, -10.0f, 10.0f))
				cam.wPos.x = fx;

			if (ImGui::SliderFloat("y", &fy, -10.0f, 10.0f))
				cam.wPos.y = fy;

			if (ImGui::SliderFloat("z", &fz, -10.0f, 10.0f))
				cam.wPos.z = fz;

			if (ImGui::Button("Button"))
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}
		ImGui::Render();
		render.display(viewMat, &cam.wPos);
	}

	render.shutdown();
	gs::shutdown();
}
