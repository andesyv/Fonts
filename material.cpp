#include "material.h"
#include <glad/glad.h>
#include <iostream>

std::weak_ptr<Shader> Material::mLastShader = std::weak_ptr<Shader>{};

Material::Material() : mShader{ShaderManager::get().defaultShader()}
{
}

Material::Material(std::string shaderName) : mShader{ShaderManager::get().findOrDefault(shaderName)}
{

}

void Material::use()
{
	mSameShader = !mLastShader.expired() && mLastShader.lock().get() == mShader.get();
	const auto shaderId = mShader->get();
	if (!mSameShader) {
		glUseProgram(shaderId);
		std::cout << "Activated shader " << shaderId << std::endl;
	}

	// Send material parameters to shader here

	// Set last used shader to be this ones.
	mLastShader = mShader;
}

bool Material::sameShaderAsLastMaterial() const
{
	return mSameShader;
}

bool Material::operator<(const Material& other) const
{
	return mShader->get() < other.mShader->get();
}

bool Material::operator>(const Material& other) const
{
	return mShader->get() > other.mShader->get();
}

bool Material::operator>=(const Material& other) const
{
	return !(*this < other);
}

bool Material::operator<=(const Material& other) const
{
	return !(*this > other);
}
