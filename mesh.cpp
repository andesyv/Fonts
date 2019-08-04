#include "mesh.h"

Vertex::Vertex(gsl::vec3 _xyz, gsl::vec3 _normal, gsl::vec2 _uv) : xyz{ _xyz }, normal{ _normal }, uv{ _uv }
{
}

Mesh::Mesh(Material material)
	: mMaterial{ material }
{
	glGenVertexArrays(1, &mVAO);
	mEnabledComponents += static_cast<char>(COMPONENT::VAO);
	glBindVertexArray(mVAO);
}

Mesh::Mesh(std::vector<Vertex> vertices, Material material)
	: mMaterial{material}, mVertexCount{static_cast<unsigned int>(vertices.size())}
{
	glGenVertexArrays(1, &mVAO);
	mEnabledComponents += static_cast<char>(COMPONENT::VAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	mEnabledComponents += static_cast<char>(COMPONENT::VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	
	// First attribute pointer: vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(0));
	glEnableVertexAttribArray(0);
	
	// Second attribute pointer: normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	// Thir attribute pointer: uvs
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<std::array<int, 3>> indices, Material material)
	: mMaterial{material}, mVertexCount{static_cast<unsigned int>(vertices.size())}
{
	glGenVertexArrays(1, &mVAO);
	mEnabledComponents += static_cast<char>(COMPONENT::VAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	mEnabledComponents += static_cast<char>(COMPONENT::VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &mEBO);
	mEnabledComponents += static_cast<char>(COMPONENT::EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int) * 3, indices.data(), GL_STATIC_DRAW);
	
	// First attribute pointer: vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(0));
	glEnableVertexAttribArray(0);

	// Second attribute pointer: normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(gsl::vec3) * 3));
	glEnableVertexAttribArray(1);

	// Thir attribute pointer: uvs
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(gsl::vec3) * 6));
	glEnableVertexAttribArray(2);
}

void Mesh::draw() const
{
	glBindVertexArray(mVAO);

	switch (mDrawMode) {
		case GL_POINTS:
		case GL_LINE_STRIP:
		case GL_LINE_LOOP:
		case GL_LINES:
		case GL_LINE_STRIP_ADJACENCY:
		case GL_LINES_ADJACENCY:
		case GL_TRIANGLE_STRIP:
		case GL_TRIANGLE_FAN:
		case GL_TRIANGLES:
		case GL_TRIANGLE_STRIP_ADJACENCY:
		case GL_TRIANGLES_ADJACENCY:
		case GL_PATCHES:
			if (mEnabledComponents & static_cast<char>(COMPONENT::EBO))
				glDrawElements(mDrawMode, mIndexCount, GL_UNSIGNED_INT, 0);
			else
				glDrawArrays(mDrawMode, 0, mVertexCount);
			break;
		default:
			break;
	}
}

bool Mesh::operator<(const Mesh& other) const
{
	return mMaterial < other.mMaterial;
}

bool Mesh::operator>(const Mesh& other) const
{
	return mMaterial > other.mMaterial;
}

bool Mesh::operator<=(const Mesh& other) const
{
	return !(*this < other);
}

bool Mesh::operator>=(const Mesh& other) const
{
	return !(*this > other);
}

Mesh::~Mesh()
{
	if (mEnabledComponents & static_cast<char>(COMPONENT::EBO))
		glDeleteBuffers(1, &mEBO);
	if (mEnabledComponents & static_cast<char>(COMPONENT::VBO))
		glDeleteBuffers(1, &mVBO);
	if (mEnabledComponents & static_cast<char>(COMPONENT::VAO))
		glDeleteBuffers(1, &mVAO);
}
