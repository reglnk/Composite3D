#include <Core/GLShaderProgram.hpp>
#include <Core/FileSystem.hpp>

#include <fstream>
#include <iterator>

#define ERROR_LOG_LENGTH 1024

// @todo support #pragma cm3d_include <path>

namespace cm3d {

GLShaderProgram::GLShaderProgram()
{
}

GLShaderProgram::GLShaderProgram(const char *vs_path, const char *fs_path, const char *gs_path)
{
	Create();
}

GLuint GLShaderProgram::Create()
{
	return Id = glCreateProgram();
}

GLuint GLShaderProgram::getId()
{
	return Id;
}

void GLShaderProgram::Use()
{
	glUseProgram(Id);
}

// Links program and checks for errors
GLint GLShaderProgram::doLink()
{
	GLint errlevel = 0;
	for (auto &pair: selectedShaders)
	{
		if (!compiledShaders.count(pair.first)) {
			errlevel |= 1;
			continue;
		}
		
		auto &sh_list = compiledShaders[pair.first];
		if (sh_list.empty()) {
			errlevel |= 2;
			continue;
		}
		if (pair.second <= sh_list.size()) 
			pair.second = 0;
		
		glAttachShader(Id, sh_list[pair.second]);
	}
	uniformLocations.clear();
	glLinkProgram(Id);
	
	GLint state;
	glGetProgramiv(Id, GL_LINK_STATUS, &state);
	if (!state)
	{
		GLchar errlog[ERROR_LOG_LENGTH];
		glGetProgramInfoLog(Id, ERROR_LOG_LENGTH, NULL, errlog);
		std::ofstream logfile("latest.log", std::fstream::app);
		if (logfile.is_open())
		{
			logfile << "Shader program failed to link:\n"
			<< errlog
			<< "\n_________________________________________________\n\n";
			logfile.close();
		}
		return errlevel | 0x8000;
	}
	return 0;
}

GLint GLShaderProgram::Compile(GLenum type, const std::vector<const GLchar *> &src)
{
	GLuint newShader = glCreateShader(type);
	Header.insert(Header.cend(), src.cbegin(), src.cend());
	glShaderSource(newShader, Header.size(), Header.data(), NULL);
	glCompileShader(newShader);

	GLint status;
	glGetShaderiv(newShader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		GLchar errlog[ERROR_LOG_LENGTH];
		glGetShaderInfoLog(newShader, ERROR_LOG_LENGTH, NULL, errlog);
		std::ofstream logfile(logPath, std::fstream::app);
		if (logfile.is_open())
		{
			logfile << "Failed to compile shader of type " << type << ":\n"
			<< errlog
			<< "\n_________________________________________________\n\n";
			logfile.close();
		}
		glDeleteShader(newShader);
	}
	else compiledShaders[type].push_back(newShader);

	return !status;
}

GLint GLShaderProgram::Compile(GLenum type, const GLchar *src)
{
	return Compile(type, std::vector<const GLchar *> {src});
}

GLint GLShaderProgram::Compile(const char *path, GLenum type)
{
	sSize fsize = 0;//FileSystem::getSize(path);
	if (fsize < 0)
		return -1;

	GLchar *src = new GLchar[fsize];
	sSize rdstatus;

	std::ifstream fish(path, std::ifstream::in);
	if (fish.fail())
		return -2;
	fish.seekg(0);
	fish.read(src, fsize);
	if (fish.fail())
		return -4;
	fish.close();

	GLint res = Compile(type, src);
	delete[] src;

	return res;
}

bool GLShaderProgram::selectShader(GLenum type, unsigned int index)
{
	/*
	if (compiledShaders.count(type) == 0
	||	compiledShaders[type].size() <= index)
		return false;
	*/
	selectedShaders[type] = index;
	return true;
}

// Detach shader if necessary, then delete
void GLShaderProgram::deleteShader(GLenum type, unsigned int index)
{
	// @OPT
	// Is this necessary?
	if (compiledShaders.count(type) <= 0
	||	compiledShaders[type].size() <= index)
		return;

	// Check for whether deleted shader was at (index) isn't necessary,
	// it'll be checked while linking.
	auto it = compiledShaders[type].begin();
	std::advance(it, index);
	compiledShaders[type].erase(it);
}

/*
 * Uniform setters
 * ========================================== */

void GLShaderProgram::setInt(const GLchar *name, const GLint val) {
	glUniform1i(getUniformLocation(name), val);
}
void GLShaderProgram::setUint(const GLchar *name, const GLuint val) {
	glUniform1ui(getUniformLocation(name), val);
}
void GLShaderProgram::setFloat(const GLchar *name, const GLfloat val) {
	glUniform1f(getUniformLocation(name), val);
}

void GLShaderProgram::setIvec2(const GLchar *name, const glm::ivec2 &val) {
	glUniform2i(getUniformLocation(name), val.x, val.y);
}
void GLShaderProgram::setIvec3(const GLchar *name, const glm::ivec3 &val) {
	glUniform3i(getUniformLocation(name), val.x, val.y, val.z);
}
void GLShaderProgram::setIvec4(const GLchar *name, const glm::ivec4 &val) {
	glUniform4i(getUniformLocation(name), val.x, val.y, val.z, val.w);
}

void GLShaderProgram::setUvec2(const GLchar *name, const glm::uvec2 &val) {
	glUniform2ui(getUniformLocation(name), val.x, val.y);
}
void GLShaderProgram::setUvec3(const GLchar *name, const glm::uvec3 &val) {
	glUniform3ui(getUniformLocation(name), val.x, val.y, val.z);
}
void GLShaderProgram::setUvec4(const GLchar *name, const glm::uvec4 &val) {
	glUniform4ui(getUniformLocation(name), val.x, val.y, val.z, val.w);
}

void GLShaderProgram::setVec2(const GLchar *name, const glm::vec2 &val) {
	glUniform2f(getUniformLocation(name), val.x, val.y);
}
void GLShaderProgram::setVec3(const GLchar *name, const glm::vec3 &val) {
	glUniform3f(getUniformLocation(name), val.x, val.y, val.z);
}
void GLShaderProgram::setVec4(const GLchar *name, const glm::vec4 &val) {
	glUniform4f(getUniformLocation(name), val.x, val.y, val.z, val.w);
}

void GLShaderProgram::setMat2(const GLchar *name, const glm::mat2 *val, size_t count) {
	glUniformMatrix2fv(getUniformLocation(name), count, GL_FALSE, (const GLfloat *)val);
}
void GLShaderProgram::setMat3x3(const GLchar *name, const glm::mat3 *val, size_t count) {
	glUniformMatrix3fv(getUniformLocation(name), count, GL_FALSE, (const GLfloat *)val);
}
void GLShaderProgram::setMat4x4(const GLchar *name, const glm::mat4 *val, size_t count) {
	glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, (const GLfloat *)val);
}


#ifdef CM3D_GL_SHADER_EXTRA_IO

void GLShaderProgram::getIntArr(const GLchar *name, GLint *params) {
	glGetUniformiv(Id, getUniformLocation(name), params);
}
void GLShaderProgram::getUintArr(const GLchar *name, GLuint *params) {
	glGetUniformuiv(Id, getUniformLocation(name), params);
}
void GLShaderProgram::getFloatArr(const GLchar *name, GLfloat *params) {
	glGetUniformfv(Id, getUniformLocation(name), params);
}

#endif

/*
 * Private functions
 * ========================================== */

GLint GLShaderProgram::getUniformLocation(const GLchar *uf) {
	if (uniformLocations.count(uf) > 0)
		return uniformLocations[uf];
	
	GLint loc = glGetUniformLocation(Id, uf);
	uniformLocations[uf] = loc;
	return loc;
}

} // namespace cm3d
