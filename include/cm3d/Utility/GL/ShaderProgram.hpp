#ifndef CM3D_UTILITY_GL_SHADER_PROGRAM_HPP
#define CM3D_UTILITY_GL_SHADER_PROGRAM_HPP

#include <cm3d/Core/FileSystem.hpp>

#include <cm3d/Tl/Aliases.hpp>
#include <cm3d/Tl/DynArray.hpp>
#include <cm3d/Tl/String.hpp>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <unordered_map>
#include <stdexcept>

namespace cm3d::Utility::GL
{
	class ShaderProgram
	{
	protected:
		GLuint Id;
		
		// Stores lines to insert at begin of shader text
		DynArray<String> defHeader;
		
		// Stores IDs of shaders for linking
		std::map<GLenum, DynArray<GLuint>> compiledShaders;
		
		// Cached uniform locations
		std::unordered_map<String, GLint> uniformLocations;

	public:
		inline ShaderProgram() = default;
		inline ShaderProgram(ShaderProgram const &other) = delete;
		inline ShaderProgram(ShaderProgram &&rv) = delete;
		
		inline ~ShaderProgram() {
			for (auto el: compiledShaders) {
				auto &stage = el.second;
				for (auto &shaderId: stage)
					glDeleteShader(shaderId);
			}
			glDeleteProgram(Id);
		}
		
		inline GLuint getId() {
			return Id;
		}
		inline void create() {
			Id = glCreateProgram();
		}
		inline void select() {
			glUseProgram(Id);
		}

		inline std::map<GLenum, DynArray<GLuint>> &getShaders() {
			return compiledShaders;
		}
		inline std::unordered_map<String, GLint> &getUniformLocCache() {
			return uniformLocations;
		}
		
		GLint getUniformLocation(const GLchar *uf) {
			if (uniformLocations.count(uf))
				return uniformLocations[uf];
			
			GLint loc = glGetUniformLocation(Id, uf);
			uniformLocations[uf] = loc;
			return loc;
		}

		// Compile the shader and mark for linking
		bool compile (
			GLenum type,
			const GLchar *src, const GLint srclen,
			String *output = nullptr,
			const char *filepath = nullptr
		);
		
		// Link all compiled shaders into the program
		bool link(String *output = nullptr);
		
		// Detach shader if necessary, then delete
		inline void deleteShader(GLenum type, unsigned int index)
		{
			CM3D_ASSERT(compiledShaders.count(type) && index < compiledShaders[type].length());

			// Check if the shader is necessary
			auto it = compiledShaders[type].begin() + index;
			glDeleteShader(*it);
			compiledShaders[type].erase(it);
		}
		
		/*
		* Uniform setters
		* ========================================== */
		
		inline void setInt(const GLchar *name, const GLint val) {
			glUniform1i(getUniformLocation(name), val);
		}
		inline void setUint(const GLchar *name, const GLuint val) {
			glUniform1ui(getUniformLocation(name), val);
		}
		inline void setFloat(const GLchar *name, const GLfloat val) {
			glUniform1f(getUniformLocation(name), val);
		}

		inline void setIvec2(const GLchar *name, const glm::ivec2 &val) {
			glUniform2i(getUniformLocation(name), val.x, val.y);
		}
		inline void setIvec3(const GLchar *name, const glm::ivec3 &val) {
			glUniform3i(getUniformLocation(name), val.x, val.y, val.z);
		}
		inline void setIvec4(const GLchar *name, const glm::ivec4 &val) {
			glUniform4i(getUniformLocation(name), val.x, val.y, val.z, val.w);
		}

		inline void setUvec2(const GLchar *name, const glm::uvec2 &val) {
			glUniform2ui(getUniformLocation(name), val.x, val.y);
		}
		inline void setUvec3(const GLchar *name, const glm::uvec3 &val) {
			glUniform3ui(getUniformLocation(name), val.x, val.y, val.z);
		}
		inline void setUvec4(const GLchar *name, const glm::uvec4 &val) {
			glUniform4ui(getUniformLocation(name), val.x, val.y, val.z, val.w);
		}

		inline void setVec2(const GLchar *name, const glm::vec2 &val) {
			glUniform2f(getUniformLocation(name), val.x, val.y);
		}
		inline void setVec3(const GLchar *name, const glm::vec3 &val) {
			glUniform3f(getUniformLocation(name), val.x, val.y, val.z);
		}
		inline void setVec4(const GLchar *name, const glm::vec4 &val) {
			glUniform4f(getUniformLocation(name), val.x, val.y, val.z, val.w);
		}

		inline void setMat2(const GLchar *name, const glm::mat2 *val, size_t count = 1) {
			glUniformMatrix2fv(getUniformLocation(name), count, GL_FALSE, (const GLfloat *)val);
		}
		inline void setMat3x3(const GLchar *name, const glm::mat3 *val, size_t count = 1) {
			glUniformMatrix3fv(getUniformLocation(name), count, GL_FALSE, (const GLfloat *)val);
		}
		inline void setMat4x4(const GLchar *name, const glm::mat4 *val, size_t count = 1) {
			glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, (const GLfloat *)val);
		}

		/*
		* Uniform getters
		* ========================================== */
		
		void getIntArr(const GLchar *name, GLint *params) {
			glGetUniformiv(Id, getUniformLocation(name), params);
		}
		void getUintArr(const GLchar *name, GLuint *params) {
			glGetUniformuiv(Id, getUniformLocation(name), params);
		}
		void getFloatArr(const GLchar *name, GLfloat *params) {
			glGetUniformfv(Id, getUniformLocation(name), params);
		}
		
		static inline constexpr const char *getShaderTypeName(GLenum type)
		{
			switch (type)
			{
				case GL_VERTEX_SHADER:
					return "Vertex";
				case GL_FRAGMENT_SHADER:
					return "Fragment";
				case GL_GEOMETRY_SHADER:
					return "Geometry";
#			ifdef GL_TESS_CONTROL_SHADER
				case GL_TESS_CONTROL_SHADER:
					return "TessControl";
				case GL_TESS_EVALUATION_SHADER:
					return "TessEvaluation";
#			endif					
#			ifdef GL_COMPUTE_SHADER
				case GL_COMPUTE_SHADER:
					return "Compute";
#			endif
				default:
					return "";
			}
		}
	};
} // namespace cm3d

#endif
