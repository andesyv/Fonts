#include "shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{
	// 1. Read shader code:
	std::ifstream vFileStream, fFileStream;
	vFileStream.open(vertexPath, std::ios_base::in);
	fFileStream.open(fragmentPath);

	// Print errors:
	if (!vFileStream) {
		std::cout << "ERROR SHADER FILE " << vertexPath << " NOT SUCCESFULLY READ" << std::endl;
	}

	if (!fFileStream) {
		std::cout << "ERROR SHADER FILE " << fragmentPath << " NOT SUCCESFULLY READ" << std::endl;
	}

	// Read content of file streams into string streams.
	std::stringstream vStrStream, fStrStream;
	vStrStream << vFileStream.rdbuf();
	fStrStream << fFileStream.rdbuf();

	// Remember to close files
	vFileStream.close();
	fFileStream.close();

	// Save contents of files into c strings:
	auto vStr{ vStrStream.str() };
	auto fStr{ fStrStream.str() };
	const GLchar* vShaderCode = vStr.c_str();
	const GLchar* fShaderCode = fStr.c_str();



	// 2. Compile shader code:
	GLuint vShader, fShader;
	GLint success;
	GLchar infoLog[512];

	// Vertex shader
	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vShaderCode, nullptr);
	glCompileShader(vShader);

	// Print compilation errors if any
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vShader, 512, nullptr, infoLog);
		std::cout << "ERROR VERTEX SHADER " << vertexPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fShaderCode, nullptr);
	glCompileShader(fShader);

	// Print compilation errors if any
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fShader, 512, nullptr, infoLog);
		std::cout << "ERROR FRAGMENT SHADER " << fragmentPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	mProgram = glCreateProgram();
	glAttachShader(mProgram, vShader);
	glAttachShader(mProgram, fShader);
	glLinkProgram(mProgram);

	// Print linking errors, if any:
	glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(mProgram, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete vertex and fragment shaders as they aren't needed anymore:
	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

Shader::Shader(std::string vertexPath, std::string geometryPath, std::string fragmentPath)
{
	// 1. Read shader code:
	std::ifstream vFileStream, gFileStream, fFileStream;
	vFileStream.open(vertexPath);
	gFileStream.open(geometryPath);
	fFileStream.open(fragmentPath);

	// Print errors:
	if (!vFileStream) {
		std::cout << "ERROR SHADER FILE " << vertexPath << " NOT SUCCESFULLY READ" << std::endl;
	}

	if (!gFileStream) {
		std::cout << "ERROR SHADER FILE " << geometryPath << " NOT SUCCESFULLY READ" << std::endl;
	}

	if (!fFileStream) {
		std::cout << "ERROR SHADER FILE " << fragmentPath << " NOT SUCCESFULLY READ" << std::endl;
	}

	// Read content of file streams into string streams.
	std::stringstream vStrStream, gStrStream, fStrStream;
	vStrStream << vFileStream.rdbuf();
	gStrStream << gFileStream.rdbuf();
	fStrStream << fFileStream.rdbuf();

	// Remember to close files
	vFileStream.close();
	gFileStream.close();
	fFileStream.close();

	// Save contents of files into c strings:
	auto vStr{ vStrStream.str() };
	auto gStr{ gStrStream.str() };
	auto fStr{ fStrStream.str() };
	const GLchar* vShaderCode = vStr.c_str();
	const GLchar* gShaderCode = gStr.c_str();
	const GLchar* fShaderCode = fStr.c_str();



	// 2. Compile shader code:
	GLuint vShader, gShader, fShader;
	GLint success;
	GLchar infoLog[512];

	// Vertex shader
	vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vShaderCode, nullptr);
	glCompileShader(vShader);

	// Print compilation errors if any
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vShader, 512, nullptr, infoLog);
		std::cout << "ERROR VERTEX SHADER " << vertexPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Geometric shader
	gShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gShader, 1, &gShaderCode, nullptr);
	glCompileShader(gShader);

	// Print compilation errors if any
	glGetShaderiv(gShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(gShader, 512, nullptr, infoLog);
		std::cout << "ERROR GEOMETRY SHADER " << geometryPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fShaderCode, nullptr);
	glCompileShader(fShader);

	// Print compilation errors if any
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fShader, 512, nullptr, infoLog);
		std::cout << "ERROR FRAGMENT SHADER " << fragmentPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	mProgram = glCreateProgram();
	glAttachShader(mProgram, vShader);
	glAttachShader(mProgram, gShader);
	glAttachShader(mProgram, fShader);
	glLinkProgram(mProgram);

	// Print linking errors, if any:
	glGetProgramiv(mProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(mProgram, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete vertex and fragment shaders as they aren't needed anymore:
	glDeleteShader(vShader);
	glDeleteShader(gShader);
	glDeleteShader(fShader);
}



std::string Shader::generateName()
{
	static unsigned int _id{ 0 };
	return std::string{ "shader" + _id };
}

GLuint Shader::get()
{
	return mProgram;
}

Shader& Shader::setName(std::string name)
{
	mName = name;
	return *this;
}

Shader::~Shader()
{
	/// I think this is how you delete shader programs...
	glDeleteProgram(mProgram);
}




//********************* ShaderManager ******************************

ShaderManager::ShaderManager()
{
	
}

ShaderManager& ShaderManager::get()
{
	// instance is created only once for the whole program.
	static ShaderManager instance{};
	return instance;
}

void ShaderManager::add(Shader&& shader)
{
	mShaders.push_back(std::make_shared<Shader>(shader));
}

std::shared_ptr<Shader> ShaderManager::make(Shader&& shader)
{
	mShaders.push_back(std::make_shared<Shader>(shader));
	return mShaders.back();
}

std::shared_ptr<Shader> ShaderManager::find(std::string shaderName)
{
	for (auto shader : mShaders)
		if (shader->mName == shaderName)
			return shader;

	return std::shared_ptr<Shader>{};
}

std::shared_ptr<Shader> ShaderManager::findOrDefault(std::string shaderName)
{
	for (auto shader : mShaders)
		if (shader->mName == shaderName)
			return shader;

	return defaultShader();
}

std::shared_ptr<Shader> ShaderManager::defaultShader()
{
	assert(mShaders.size() > 0);
		return mShaders.front();
}

ShaderManager::~ShaderManager()
{
	std::cout << "Destroying " << mShaders.size() << " shaders" << std::endl;
	for (auto& item : mShaders)
		if (item.use_count() != 1)
			std::cout << "Shader " << item->mName << " is still being managed by another shared pointer." << std::endl;

}
