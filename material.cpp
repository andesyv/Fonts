#include "material.h"
#include <glad/glad.h>
#include <iostream>
#include <type_traits>

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
	if (mParams & static_cast<unsigned char>(PARAM::AMBIENT))
		glUniform1f(glGetUniformLocation(shaderId, "ambient"), mAmbient);
	if (mParams & static_cast<unsigned char>(PARAM::DIFFUSE))
		glUniform1f(glGetUniformLocation(shaderId, "diffuse"), mDiffuse);
	if (mParams & static_cast<unsigned char>(PARAM::SPECULAR))
		glUniform1f(glGetUniformLocation(shaderId, "specular"), mSpecular);
	if (mParams & static_cast<unsigned char>(PARAM::COLOR))
		glUniform4fv(glGetUniformLocation(shaderId, "color"), 1, mColor.xP());
	if (mParams & static_cast<unsigned char>(PARAM::TEXTURE))
		glBindTexture(GL_TEXTURE_2D, mTextureID);
	if (mParams & static_cast<unsigned char>(PARAM::UVSCALE))
		glUniform2fv(glGetUniformLocation(shaderId, "uvScale"), 1, mUVScale.xP());
	if (mParams & static_cast<unsigned char>(PARAM::UVPOS))
		glUniform2fv(glGetUniformLocation(shaderId, "uvPos"), 1, mUVPos.xP());

	// Set last used shader to be this ones.
	mLastShader = mShader;
}

bool Material::sameShaderAsLastMaterial() const
{
	return mSameShader;
}

void Material::enableParams(Material::PARAM flags)
{
	mParams |= static_cast<unsigned char>(flags);
}

void Material::disableParams(Material::PARAM flags)
{
	// 0b11111111 is a 1 byte unsigned integer maximum number (255)
	mParams &= 0b11111111 ^ static_cast<unsigned char>(flags);
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

Material::PARAM operator | (Material::PARAM lhs, Material::PARAM rhs)
{
	using T = std::underlying_type_t <Material::PARAM>;
	return static_cast<Material::PARAM>(static_cast<T>(lhs) | static_cast<T>(rhs));
}