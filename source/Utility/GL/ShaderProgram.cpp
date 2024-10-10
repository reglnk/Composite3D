#include <cm3d/Core/FileSystem.hpp>
#include <cm3d/Utility/GL/ShaderProgram.hpp>
#include <cm3d/Utility/GL/Preprocessor.hpp>

#include <fstream>
#include <iterator>

#define SHPROG_NS "cm3d::Utility::GL::ShaderProgram"

namespace cm3d::Utility::GL
{
	bool ShaderProgram::compile (
		GLenum type,
		const GLchar *src, const GLint srclen,
		String *output,
		const char *filepath
	) {
		CM3D_ASSERT(src);
		CM3D_ASSERT(srclen);
		
		DynArray<GLchar const *> strings;
		DynArray<GLint> lens;
		
		// Add configurations
		for (size_t i = 0; i < defHeader.length(); ++i) {
			strings.pushBack(defHeader[i].c_str());
			lens.pushBack(defHeader[i].length());
		}
		
		// Prepare the main source
		DynArray<char> srcPreprocessed;
		FileSystem::sPath filewd;
		if (filepath) {
			filewd = filepath;
			FileSystem::parentDir(&filewd);
		} else filewd = ".";
		
		Preprocessor::processSource(src, src + srclen, &srcPreprocessed, filewd);
		strings.pushBack(srcPreprocessed.begin());
		lens.pushBack(srcPreprocessed.size());
		
		// Create & compile the shader
		GLuint new_shader = glCreateShader(type);
		if (!new_shader) {
			if (output) {
				*output += SHPROG_NS "::compile: error\nFailed to create shader [";
				*output += getShaderTypeName(type);
				*output += "]\n";
			}
			return false;
		}
		glShaderSource(new_shader, strings.length(), strings.data(), lens.data());
		glCompileShader(new_shader);

		GLint status;
		glGetShaderiv(new_shader, GL_COMPILE_STATUS, &status);
		if (status == GL_TRUE) {
			compiledShaders[type].pushBack(new_shader);
			return true;
		}
		if (output)
		{
			*output += SHPROG_NS "::compile: error\nShader compilation failed [";
			*output += getShaderTypeName(type);
			if (filepath)
			{
				*output += ", \"";
				*output += filepath;
				*output += "\"]\n";
			}
			
			GLint log_buf_size;
			glGetShaderiv(new_shader, GL_INFO_LOG_LENGTH, &log_buf_size);
			
			if (log_buf_size)
			{
				size_t cur_len = output->length();
				output->resize(cur_len + log_buf_size - 1);
				glGetShaderInfoLog(new_shader, log_buf_size, NULL, output->begin() + cur_len);
			}
			*output += "\n";
		}
		return false;
	}

	// Link all compiled shaders into the program
	bool ShaderProgram::link(String *output)
	{
		for (auto el: compiledShaders) {
			auto &stage = el.second;
			for (auto &shaderId: stage)
				glAttachShader(Id, shaderId);
		}
		
		// clear cache after previous program usage
		uniformLocations.clear();
		glLinkProgram(Id);
		
		for (auto el: compiledShaders) {
			auto &stage = el.second;
			for (auto &shaderId: stage)
				glDetachShader(Id, shaderId);
		}
		
		GLint state;
		glGetProgramiv(Id, GL_LINK_STATUS, &state);
		if (state)
			return true;
		
		if (output)
		{
			GLint log_buf_size;
			glGetProgramiv(Id, GL_INFO_LOG_LENGTH, &log_buf_size);
			
			if (log_buf_size)
			{
				size_t cur_len = output->length();
				output->resize(cur_len + log_buf_size - 1);
				glGetProgramInfoLog(Id, log_buf_size, NULL, output->begin() + cur_len);
			}
			*output += "\n";
		}
		return false;
	}
}
