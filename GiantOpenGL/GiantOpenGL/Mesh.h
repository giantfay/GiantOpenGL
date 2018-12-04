#pragma once

#include "glad/glad.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "Shader.h"

#include <string>
#include <vector>


enum class TextureType
{
	DIFFUSE,
	SPECULAR,
};

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	GLuint id;
	TextureType type;
};

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures)
		:Vertices(vertices), Indices(indices), Textures(textures)
	{
		setupMesh();
	}

	void Draw(Shader shader) const;
	
public:
	std::vector<Vertex> Vertices;
	std::vector<GLuint> Indices;
	std::vector<Texture> Textures;

private:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

private:
	void setupMesh();
};

void Mesh::Draw(Shader shader) const
{
	unsigned int diffuseNum = 1;
	unsigned int specularNum = 1;
	for (unsigned int i = 0; i < Textures.size(); ++i)
	{
		std::string texName = "";
		switch (Textures[i].type)
		{
		case TextureType::DIFFUSE:
			texName = "texture_diffuse" + std::to_string(diffuseNum++);
			break;
		case TextureType::SPECULAR:
			texName = "texture_specular" + std::to_string(specularNum++);
			break;
		default:
			break;
		}
		shader.SetInt(texName, i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, Textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size()*sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
	//vertex layout
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoords)));
	glEnableVertexAttribArray(2);
	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size()*sizeof(GLuint), &Indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

