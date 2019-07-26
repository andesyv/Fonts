#pragma once
#include <glad/glad.h>

class Shader
{
private:
	GLuint mProgram;

public:
	Shader(const char vertexPath[], const char fragmentPath[]);
	Shader(const char vertexPath[], const char geometricPath[], const char fragmentPath[]);

	~Shader();
};

/** Singleton shader manager class
 * Class that is instantiated once and keeps track of 
 * and manages shader programs
 */
class ShaderManager {

};