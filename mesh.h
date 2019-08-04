#pragma once
#include <glad/glad.h>
#include <vector>
#include <array>
#include "Math/vector2d.h"
#include "Math/vector3d.h"
#include "material.h"

struct Vertex {
	gsl::vec3 xyz;
	gsl::vec3 normal;
	gsl::vec2 uv;

	Vertex(gsl::vec3 _xyz = gsl::vec3{ 0.f, 0.f, 0.f }, gsl::vec3 _normal = gsl::vec3{ 0.f, 0.f, 0.f }, gsl::vec2 _uv = gsl::vec2{ 0.f, 0.f });
};

class Mesh
{
private:
	enum class COMPONENT : char{
		VAO = 1,
		VBO = 2,
		EBO = 4
	};

	// Which variables are in use (initialized)
	char mEnabledComponents{ 0 };

	GLuint mVBO{ 0 };
	GLuint mEBO{ 0 };
	unsigned int mVertexCount{ 0 };
	unsigned int mIndexCount{ 0 };

public:
	GLuint mVAO{ 0 };
	Material mMaterial{};
	GLenum mDrawMode{ GL_TRIANGLES };

	Mesh(Material material = Material{});
	Mesh(std::vector<Vertex> vertices, Material material = Material{});
	Mesh(std::vector<Vertex> vertices, std::vector<std::array<int, 3>> indices, Material material = Material{});

	void draw() const;

	bool operator< (const Mesh& other) const;
	bool operator> (const Mesh& other) const;
	bool operator<= (const Mesh& other) const;
	bool operator>= (const Mesh& other) const;

	~Mesh();
};

