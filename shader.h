#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include <memory>
#include <cstdarg>

class Shader
{
private:
	GLuint mProgram;

	static std::string generateName();

public:
	std::string mName;

	Shader(std::string vertexPath, std::string fragmentPath, std::string name = generateName());
	Shader(std::string vertexPath, std::string geometricPath, std::string fragmentPath, std::string name = generateName());

	GLuint get();

	~Shader();
};

/** Singleton shader manager class
 * Class that is instantiated once and keeps track of 
 * and manages shader programs
 *
 * Idea courtesy of SPACEDANCE
 */
class ShaderManager {
private:
	std::vector<std::shared_ptr<Shader>> mShaders;
	ShaderManager();

public:
	static ShaderManager& get();

	/** Template function to just forward multiple arguments
	 * to the std::make_shared function.
	 * Arguments are: vertexPath <, geometricPath>, fragmentPath <, name> 
	 * (ellipsis's are somewhat dangerous)
	 */
	template <class... _Types>
	void add(_Types&& ... args) {
		mShaders.push_back(std::make_shared<Shader>(args));
	}
	std::shared_ptr<Shader> find(std::string shaderName);

	~ShaderManager();
};