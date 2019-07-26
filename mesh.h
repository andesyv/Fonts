#pragma once
#include <glad/glad.h>
#include <vector>
#include "Math/vector2d.h"
#include "Math/vector3d.h"

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

	GLuint mVBO;
	GLuint mEBO;
	unsigned int mVertexCount{ 0 };
	unsigned int mIndexCount{ 0 };

public:
	GLuint mVAO;
	GLenum mDrawMode{ GL_TRIANGLES };

	Mesh();
	Mesh(std::vector<Vertex> vertices);
	Mesh(std::vector<Vertex> vertices, std::vector<std::array<int, 3>> indices);

	void draw() const;

	~Mesh();

};

