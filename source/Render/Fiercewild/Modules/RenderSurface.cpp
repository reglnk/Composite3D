#include "cm3d/Core/FileSystem.hpp"
#include <cm3d/Render/Fiercewild/Modules/RenderSurface.hpp>
#include <cm3d/Utility/GL/Preprocessor.hpp>

using namespace cm3d::FileSystem;

namespace cm3d::Render::Fiercewild
{
	MeshRenderData::MeshRenderData(Graphics::Mesh const &msh): matIndex(msh.getMatIndex()), facesCount(msh.getFaces().length())
	{
		if (!facesCount)
			return;
		
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		
		auto const &faces = msh.getFaces();
		
		if (msh.getVertices().length())
		{
			auto const &verts = msh.getVertices();
			glGenBuffers(1, &VBO[0]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, verts.bytecount(), verts.data(), GL_DYNAMIC_DRAW);
			
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(0);
		}
		else VBO[0] = 0;
		
		if (msh.getTexmap().length())
		{
			auto const &texmap = msh.getTexmap();
			glGenBuffers(1, &VBO[1]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, texmap.bytecount(), texmap.data(), GL_STATIC_DRAW);
			
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
			glEnableVertexAttribArray(1);
		}
		else VBO[1] = 0;
		
		if (msh.getNormals().length())
		{
			auto const &norm = msh.getNormals();
			glGenBuffers(1, &VBO[2]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
			glBufferData(GL_ARRAY_BUFFER, norm.bytecount(), norm.data(), GL_DYNAMIC_DRAW);
			
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(2);
		}
		else VBO[2] = 0;
		
		if (msh.getTangents().length())
		{
			auto const &tan = msh.getTangents();
			glGenBuffers(1, &VBO[3]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
			glBufferData(GL_ARRAY_BUFFER, tan.bytecount(), tan.data(), GL_DYNAMIC_DRAW);
			
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(3);
		}
		else VBO[3] = 0;
		
		if (msh.getBitangents().length())
		{
			auto const &bit = msh.getBitangents();
			glGenBuffers(1, &VBO[4]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
			glBufferData(GL_ARRAY_BUFFER, bit.bytecount(), bit.data(), GL_DYNAMIC_DRAW);
			
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			glEnableVertexAttribArray(4);
		}
		else VBO[4] = 0;
		
		if (msh.getVertexColors().length())
		{
			auto const &vcol = msh.getVertexColors();
			glGenBuffers(1, &VBO[5]);
			glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
			glBufferData(GL_ARRAY_BUFFER, vcol.bytecount(), vcol.data(), GL_STATIC_DRAW);
			
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
			glEnableVertexAttribArray(5);
		}
		else VBO[5] = 0;
		
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.bytecount(), faces.data(), GL_STATIC_DRAW);
		
		glBindVertexArray(0);
	}
	
	namespace Modules
	{
		RenderSurface::RenderSurface (
			cm3d::Modules::RenderSurface const *rs,
			FileSystem::sPath const &shaders_dir,
			String *output
		): rSurf(rs), textures(rSurf->getTextures().length())
		{
			matShaders.reserve(rSurf->getMaterials().length());
			for (auto const &mat: rSurf->getMaterials())
				matShaders.pushBack(createShader(mat, shaders_dir, output));

			loadTextures();
			copyRsNode(rSurf->getRoot(), &rootNode);
		}

		inline int tryShader (
			sPath const &basedir,
			sPath const &matname,
			sPath &result,
			GLenum type,
			sSize &fsize
		) {
			static const sPath def("default");
			const char *postfix =
				type == GL_VERTEX_SHADER ? "_vs.glsl" :
				type == GL_FRAGMENT_SHADER ? "_fs.glsl" :
				"_gs.glsl";

			result = FileSystem::concat(basedir, matname + postfix);
			if (!FileSystem::getFileSize(result.c_str(), &fsize))
				return true;

			result = FileSystem::concat(basedir, def + postfix);
			if (!FileSystem::getFileSize(result.c_str(), &fsize))
				return false;

			// @todo embed default shader (just copy the default shader file)
			throw std::runtime_error(std::string("Couldn't find default shader: ") + result.c_str());
		}

		Utility::GL::ShaderProgram *RenderSurface::createShader (
			Graphics::Material const &mat, sPath const &shaders_dir,
			String *output
		) {
			if (!mat.name)
				return nullptr;
			

			sSize fsize;

			auto failNotfound = [output](sPath const &path) {
				*output += String("Utility::GL::ShaderProgram: error: ") + path + " not found\n";
				return nullptr;
			};
			auto failRead = [output](sPath const &path) {
				*output += String("Utility::GL::ShaderProgram: error: unable to read") + path + "\n";
				return nullptr;
			};

			sPath vs_path;
			tryShader(shaders_dir, mat.name, vs_path, GL_VERTEX_SHADER, fsize);
			
			String vertex(fsize);
			if (FileSystem::readFile(vs_path.c_str(), reinterpret_cast<sByte *>(vertex.data()), fsize))
				return failRead(vs_path);
			
			sPath fs_path;
			tryShader(shaders_dir, mat.name, fs_path, GL_FRAGMENT_SHADER, fsize);
			
			String fragment(fsize);
			if (FileSystem::readFile(fs_path.c_str(), reinterpret_cast<sByte *>(fragment.data()), fsize))
				return failRead(fs_path);
			
			sPath basepath = FileSystem::concat(shaders_dir, mat.name);
			sPath gs_path = basepath + "_gs.glsl";
			String geometry;
			if (!FileSystem::getFileSize(gs_path.c_str(), &fsize))
			{
				geometry.resize(fsize);
				if (FileSystem::readFile(gs_path.c_str(), reinterpret_cast<sByte *>(geometry.data()), fsize))
					geometry = String();
				else *output += String("Utility::GL::ShaderProgram: ") + gs_path + " found\n";
			}
			
			auto shprog = new Utility::GL::ShaderProgram();
			shprog->create();
			
			bool success = shprog->getId()
			&&	shprog->compile(GL_VERTEX_SHADER, vertex.c_str(), vertex.length(), output, vs_path.c_str())
			&&	shprog->compile(GL_FRAGMENT_SHADER, fragment.c_str(), fragment.length(), output, fs_path.c_str())
			&&	(geometry ? shprog->compile(GL_GEOMETRY_SHADER, geometry.c_str(), geometry.length(), output, gs_path.c_str()) : true)
			&&	shprog->link(output);
			
			if (!success) {
				delete shprog;
				return nullptr;
			}
			return shprog;
		}
	}
}
