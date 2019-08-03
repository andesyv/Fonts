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
	// For use by move semantics
	// (keeps order of who "owns" the shaderprogram so that
	// it doesn't get destroyed by anybody else but the owner)
	bool mOwner{ false };

	static std::string generateName();

public:
	std::string mName;

	Shader(const Shader& other);
	Shader(Shader&& other);
	Shader(std::string vertexPath, std::string fragmentPath);
	Shader(std::string vertexPath, std::string geometryPath, std::string fragmentPath);

	Shader& operator= (const Shader& other);
	Shader& operator= (Shader&& other);

	GLuint get();
	Shader& setName(std::string name);

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

	void operator= (const ShaderManager& other) = delete;

	// Adds a shader to the shaderManager
	void add(Shader&& shader);
	// Adds a shader to the shadermanager and returns a shared_ptr to the shader.
	// (the same as ShaderManager::add but less efficient)
	std::shared_ptr<Shader> make(Shader&& shader);

	// Searches for shader and returns an empty pointer if not found
	std::shared_ptr<Shader> find(std::string shaderName);
	// Searches for shader and returns the default shader if not found
	std::shared_ptr<Shader> findOrDefault(std::string shaderName);
	/** Returns the first shader that is added (the default one).
	 * NB: Will stop the program at runtime if no shaders are found.
	 */
	std::shared_ptr<Shader> defaultShader();

	~ShaderManager();
};