#include "cm3d/Modules/Position.hpp"
#include <cm3d/Render/Fiercewild/Fiercewild.hpp>

#include <cm3d/Modules/RenderSurface.hpp>
#include <cm3d/Render/Fiercewild/Modules/RenderSurface.hpp>

#include <cm3d/AppLayer/GUI.hpp>

#include <glm/gtc/matrix_transform.hpp>

// @todo solve issue that
// if a shader for material is not found in shaders/ then its not rendered

namespace cm3d::Render::Fiercewild
{
	void Fiercewild::preconfigure()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	int Fiercewild::fnInit()
	{
		glfwMakeContextCurrent(mWindow);
		auto loadfn = reinterpret_cast<GLADloadfunc>(glfwGetProcAddress);
		if (!gladLoadGL(loadfn))
			return -1;

		// glClearColor(0.678f, 0.784f, 0.898f, 1.f);
		glClearColor(0.12f, 0.12f, 0.23f, 1.f);
		glEnable(GL_DEPTH_TEST);
		return 0;
	}

	int Fiercewild::fnShutdown() {
		return 0;
	}

	static const char *getTexUniformName(aiTextureType type)
	{
		switch (type)
		{
			case aiTextureType_BASE_COLOR:
				return "uTexBaseColor";
			case aiTextureType_AMBIENT:
				return "uTexAmbient";
			case aiTextureType_DIFFUSE:
				return "uTexDiffuse";
			case aiTextureType_SPECULAR:
				return "uTexSpecular";
			case aiTextureType_NORMALS:
				return "uTexNormals";
			case aiTextureType_EMISSIVE:
				return "uTexEmissive";
			case aiTextureType_DISPLACEMENT:
				return "uTexDisplacement";
			case aiTextureType_HEIGHT:
				return "uTexHeight";
			case aiTextureType_METALNESS:
				return "uTexMetalness";
			case aiTextureType_OPACITY:
				return "uTexOpacity";
			case aiTextureType_AMBIENT_OCCLUSION:
				return "uTexAmbientOcclusion";
			default:
				return "uTexUnknown";
		}
	}

	static void renderNode (
		rsdNode const &node,
		Modules::RenderSurface const *rs,
		glm::mat4 const &cur_transform,
		glm::mat4 const &view
	) {
		std::cout << "renderNode: ";
		cm3d::Modules::RenderSurface const *surf = rs->rSurf;

		glm::mat4 transform = node.transform * cur_transform;

		std::cout << node.numMeshes << " meshes\n";
		for (uint32_t i = 0; i != node.numMeshes; ++i)
		{
			MeshRenderData const &mrd = rs->meshesRd[i];
			Utility::GL::ShaderProgram *sp = rs->matShaders[mrd.matIndex];
			if (!sp)
				continue; // todo use fallback

			// incase viewPos is provided, we're assuming here
			//

			sp->select();
			sp->setMat4x4("uView", &view);
			sp->setMat4x4("uNodeTransform", &transform);

			auto const &mat = surf->getMaterials()[mrd.matIndex];
			sp->setVec3("uColAmbient", mat.ambient);
			sp->setVec3("uColDiffuse", mat.diffuse);
			sp->setVec3("uColSpecular", mat.specular);
			sp->setVec3("uColEmissive", mat.emissive);
			sp->setVec3("uColTransparent", mat.transparent);

			sp->setFloat("uPropOpacity", mat.opacity);
			sp->setFloat("uPropRoughness", mat.roughness);
			sp->setFloat("uPropMetalness", mat.metalness);

			// now set all textures from the material
			uint32_t texCount = 0;

			// @todo consider some solution to prevent exceeding GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
			for (uint32_t ti: mat.texIndex)
			{
				const GLuint texUnit = rs->textures[ti];
				if (!texUnit || texCount == 32)
					continue;

				glActiveTexture(GL_TEXTURE0 + texCount++);
				glBindTexture(GL_TEXTURE_2D, rs->textures[ti]);
				aiTextureType type = surf->getTextures()[ti].type();
				sp->setUint(getTexUniformName(type), rs->textures[ti]);
			}

			glBindVertexArray(mrd.VAO);
			glDrawElements(GL_TRIANGLES, mrd.facesCount * 3, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		for (uint32_t i = 0; i != node.numInner; ++i)
			renderNode(node.inner[i], rs, transform, view);
	}

	int Fiercewild::fnDisplay(glm::mat4 const &view, Vector3 const *viewPos)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto dispFn = [&](Modules::RenderSurface const *rs, cm3d::Modules::Position const *p)
		{
			std::cout << "displaying " << rs << " with " << p << '\n';
			if (viewPos != nullptr) {
				auto const &pos = p->value - *viewPos;
				auto transform = glm::translate(glm::mat<4, 4, sReal>(1.0), glm::crvec3(pos.x, pos.y, pos.z));
				renderNode(rs->rootNode, rs, transform, view);
			}
			else renderNode(rs->rootNode, rs, glm::mat4(1.f), view);
		};

		for (auto &pair: mData)
		{
			using posModule = cm3d::Modules::Position;

			auto const &mods = mWorld->objects[pair.first].getModules();
			cm3d::Modules::Position const *pos {nullptr};
			for (auto &m: mods) {
				if (m->getId() == cm3d::Modules::Position::Id) {
					pos = dynamic_cast<posModule *>(m.get());
				}
			}
			if (pos == nullptr)
				throw pos;
			for (auto &m: pair.second.modules) {
				if (m->getId() == cm3d::Modules::RenderSurface::Id) {
					dispFn(dynamic_cast<Modules::RenderSurface const *>(m.get()), pos);
					break;
				}
			}
			for (auto &m: pair.second.sharedModules) {
				if (m->getId() == cm3d::Modules::RenderSurface::Id) {
					dispFn(dynamic_cast<Modules::RenderSurface const *>(m.get()), pos);
					break;
				}
			}
		}

		app::gui::renderFinal(ImGui::GetDrawData());
		glfwSwapBuffers(mWindow);
		return 0;
	}

	void Fiercewild::syncFramebuffer(int width, int height) {
		glViewport(0, 0, width, height);
	}

	// things to load:
	// 1. VAOs VBOs EBOs
	// 2. Textures
	// 3. Shaders
	// @see Fiercewild/Modules/RenderSurface.hpp
	// @fixme no advantage of shared modules
	int Fiercewild::fnLoad(World const &world, std::size_t id)
	{
		auto const &modules = world.objects[id].getModules();
		for (auto const &m: modules)
		{
			if (m->getId() == cm3d::Modules::RenderSurface::Id)
			{
				rs_process_fn(id, m.get());
				break;
			}
		}
		auto const &shr_modules = world.objects[id].getSharedModules();
		for (auto const &m: shr_modules)
		{
			if (m->getId() == cm3d::Modules::RenderSurface::Id)
			{
				rsshared_process_fn(id, m.get());
				break;
			}
		}
		return 0;
	}

	// @todo remove int return code and replace with exceptions (or not, if that will be inconvenient in lua)
	int Fiercewild::fnUnload(std::size_t id)
	{
		mData.erase(id);
		return 0;
	}

	void Fiercewild::rs_process_fn(std::size_t id, cm3d::Modules::Base const *m)
	{
		auto rs = reinterpret_cast<cm3d::Modules::RenderSurface const *>(m);

		String logOutput;
		mData[id].modules.pushBack (
			std::make_unique<Modules::RenderSurface> (
				rs, shadersDir, this->logMgr ? &logOutput : nullptr
			)
		);
		if (logOutput)
			logMgr->push(logOutput);
	}

	void Fiercewild::rsshared_process_fn(std::size_t id, cm3d::Modules::Base const *m)
	{
		auto rs = reinterpret_cast<cm3d::Modules::RenderSurface const *>(m);
		Object::pModShared shm;

		if (!mShared.count(m))
		{
			String logOutput;
			shm = std::make_shared<Modules::RenderSurface> (
				rs, shadersDir, this->logMgr ? &logOutput : nullptr
			);
			mShared[m] = shm;
			if (logOutput)
				logMgr->push(logOutput);
		}
		else shm = mShared[m];

		mData[id].sharedModules.pushBack(shm);
	}
}
