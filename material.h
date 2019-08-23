#pragma once
#include "shader.h"
#include "Math/vector2d.h"
#include "Math/vector3d.h"
#include "Math/vector4d.h"

class Material
{
private:
	static std::weak_ptr<Shader> mLastShader;
	bool mSameShader{ false };

public:
	enum class PARAM : unsigned char {
		AMBIENT = 1,
		DIFFUSE = 2,
		SPECULAR = 4,
		COLOR = 8,
		TEXTURE = 16,
		UVSCALE = 32,
		UVPOS = 64,
		TRANSPARANCY = 128
	};

	// Enabled parameters (others are default)
	unsigned char mParams{0};

	std::shared_ptr<Shader> mShader;
	GLuint mTextureID{ 0 };
	gsl::vec4 mColor{ 1.f, 1.f, 1.f, 1.f };
	float mAmbient{ 0.15f };
	float mDiffuse{ 0.8f };
	float mSpecular{ 0.4f };
	gsl::vec2 mUVScale{ 1.f, 1.f };
	gsl::vec2 mUVPos{ 0.f, 0.f };

public:
	Material();
	Material(std::string shaderName);
	void use();
	// Is this material using the same shader as the last material?
	bool sameShaderAsLastMaterial() const;

	void enableParams(Material::PARAM flags);
	void disableParams(Material::PARAM flags);

	bool operator< (const Material& other) const;
	bool operator> (const Material& other) const;
	bool operator>= (const Material& other) const;
	bool operator<= (const Material& other) const;

};

Material::PARAM operator | (Material::PARAM lhs, Material::PARAM rhs);

