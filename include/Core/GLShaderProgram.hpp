#ifndef CM3D_GL_SHADER_PROGRAM_HPP
#define CM3D_GL_SHADER_PROGRAM_HPP

#include <Types/Aliases.hpp>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <unordered_map>

// @todo support #pragma cm3d_include <path>


namespace cm3d
{
	class GLShaderProgram
	{
	public:
		GLShaderProgram();
		GLShaderProgram(const char *vs_path, const char *fs_path, const char *gs_path = nullptr);
		
		// Calls glCreateProgram and returns Id
		GLuint Create();
		
		// glUseProgram(Id);
		void Use();
		
		// Always returns 0 if link was successful.
		int doLink();

		// Compile array of strings (definitions applied)
		GLint Compile(GLenum type, const std::vector<const GLchar *> &src);

		// Alias for previous Compile.
		GLint Compile(GLenum type, const GLchar *src);

		// Alias for previous Compile.
		GLint Compile(const char *path, GLenum type);
		
		// Selects some shader from compiled ones. Needs doLink() to take effect
		bool selectShader(GLenum type, unsigned int index);
		
		// Detach shader if necessary, then delete
		void deleteShader(GLenum type, unsigned int index);
		
		/*
		* Uniform setters
		* ========================================== */
		
		void setInt(const GLchar *name, const GLint val);
		void setUint(const GLchar *name, const GLuint val);
		void setFloat(const GLchar *name, const GLfloat val);
		
		void setIvec2(const GLchar *name, const glm::ivec2 &val);
		void setIvec3(const GLchar *name, const glm::ivec3 &val);
		void setIvec4(const GLchar *name, const glm::ivec4 &val);
		
		void setUvec2(const GLchar *name, const glm::uvec2 &val);
		void setUvec3(const GLchar *name, const glm::uvec3 &val);
		void setUvec4(const GLchar *name, const glm::uvec4 &val);
		
		void setVec2(const GLchar *name, const glm::vec2 &val);
		void setVec3(const GLchar *name, const glm::vec3 &val);
		void setVec4(const GLchar *name, const glm::vec4 &val);
		
		void setMat2(const GLchar *name, const glm::mat2 *val, size_t count = 1);
		void setMat3x3(const GLchar *name, const glm::mat3 *val, size_t count = 1);
		void setMat4x4(const GLchar *name, const glm::mat4 *val, size_t count = 1);

	#ifdef CM3D_GL_SHADER_EXTRA_IO
		
		/*
		* Uniform getters
		* ========================================== */
		
		void getIntArr(const GLchar *name, GLint *params);
		void getUintArr(const GLchar *name, GLuint *params);
		void getFloatArr(const GLchar *name, GLfloat *params);

	#endif
		
		GLuint getId();

		// Tries to get uniform from cache. If not found, calls glGetUniformLocation.
		GLint getUniformLocation(const GLchar * uf);

		inline std::unordered_map<GLenum, std::vector<GLuint>> &getShaders() {
			return compiledShaders;
		}
		inline std::unordered_map<GLenum, GLuint> &getSelected() {
			return selectedShaders;
		}
		inline std::unordered_map<const GLchar *, GLint> &getUniformLocCache() {
			return uniformLocations;
		}
		
		// Stores lines to insert at begin of shader text.
		// Is cleared after compiling
		std::vector<const GLchar *> Header;

		// log for messages from OpenGL on fails to compile shaders
		std::string logPath;
		
	protected:
		GLuint Id;
		
		// Stores IDs of shaders for linking
		std::unordered_map<GLenum, std::vector<GLuint>> compiledShaders;
		
		// Stores indexes of selected shaders
		std::unordered_map<GLenum, unsigned> selectedShaders;
		
		// Cached uniform locations
		std::unordered_map<const GLchar *, GLint> uniformLocations;
	};
} // namespace cm3d

#endif
