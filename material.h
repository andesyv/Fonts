#pragma once
#include "shader.h"

class Material
{
public:
	std::shared_ptr<Shader> mShader;

public:
	Material(std::string shaderName);

};

