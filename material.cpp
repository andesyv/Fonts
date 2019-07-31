#include "material.h"

Material::Material(std::string shaderName) : mShader{ShaderManager::get().findOrDefault(shaderName)}
{

}
