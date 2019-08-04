#pragma once
#include "shader.h"

class Material
{
private:
	static std::weak_ptr<Shader> mLastShader;
	bool mSameShader{ false };

public:
	enum PARAMETER : unsigned char {
		COLOR = 1

	};

	std::shared_ptr<Shader> mShader;
	unsigned char mParams{};

public:
	Material();
	Material(std::string shaderName);
	void use();
	// Is this material using the same shader as the last material?
	bool sameShaderAsLastMaterial() const;

	bool operator< (const Material& other) const;
	bool operator> (const Material& other) const;
	bool operator>= (const Material& other) const;
	bool operator<= (const Material& other) const;

};

