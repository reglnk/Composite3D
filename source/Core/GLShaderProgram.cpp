#include <Core/GLShaderProgram.hpp>
#include <Core/FileSystem.hpp>

#include <fstream>
#include <iterator>

namespace cm3d
{
	bool GLShaderProgram::compile(
		GLenum type,
		const GLchar *src, const GLint srclen,
		String *output,
		const char *filewd
	) {
		CM3D_ASSERT(src);
		CM3D_ASSERT(srclen);
		
		DynArray<GLchar const *> strings;
		DynArray<GLint> lens;
		
		// Add configurations
		for (size_t i = 0; i < defHeader.length(); ++i) {
			strings.pushBack(defHeader[i].c_str());
			lens.pushBack(defHeader[i].size());
		}
		
		// Prepare the main source
		DynArray<char> srcPreprocessed;
		GLSLPreprocessor::processSource(src, src + srclen, &srcPreprocessed, filewd);
		strings.pushBack(srcPreprocessed.begin());
		lens.pushBack(srcPreprocessed.size());
		
		// Create & compile the shader
		GLuint new_shader = glCreateShader(type);
		if (!new_shader) {
			if (output)
				*output += "GLShaderProgram::compile: glCreateShader failed\n";
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
			GLint log_len;
			glGetShaderiv(new_shader, GL_INFO_LOG_LENGTH, &log_len);
			size_t cur_size = output->size();
			output->resize(cur_size + log_len);
			glGetShaderInfoLog(new_shader, log_len, NULL, output->begin() + cur_size);
		}
		return false;
	}

	// Link all compiled shaders into the program
	bool GLShaderProgram::link(String *output)
	{
		int errcode = 0;
		
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
			GLint log_len;
			glGetProgramiv(Id, GL_INFO_LOG_LENGTH, &log_len);
			output->resize(log_len);
			glGetProgramInfoLog(Id, output->size(), NULL, output->data());
		}
		return false;
	}
} // namespace cm3d
