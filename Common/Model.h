#pragma once

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "stb_image.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "Mesh.h"
#include "Shader.h"

GLuint LoadTextureFromFile(const char* path, const std::string& directory);

TextureType AiTexTypeToTexType(aiTextureType aiType);

class Model
{
public:
	Model(const std::string& path)
	{
		loadModel(path);
	}
	void Draw(Shader shader) const;

private:
	void loadModel(const std::string& path);
	void processNode(const aiNode* node, const aiScene* scene);
	Mesh processMesh(const aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, aiTextureType type);
private:
	std::vector<Mesh> meshes;
	std::string directory;

	std::map<std::string, Texture> texture_loaded;
};

void Model::Draw(Shader shader) const
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
	{
		std::cout << "Assimp error: " << importer.GetErrorString() << std::endl;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(pScene->mRootNode, pScene);
}

void Model::processNode(const aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(const aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		
		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	std::vector<GLuint> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
		{
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	std::vector<Texture> textures;
	aiMaterial* pMat = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = loadMaterialTextures(pMat, aiTextureType_DIFFUSE);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
	std::vector<Texture> specularTextures = loadMaterialTextures(pMat, aiTextureType_SPECULAR);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(const aiMaterial* mat, aiTextureType type)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString path;
		mat->GetTexture(type, i, &path);
		auto it = texture_loaded.find(std::string(path.C_Str()));
		if (it != texture_loaded.end()) // has loaded
		{
			textures.push_back(it->second);
		}
		else
		{
			Texture texture;
			texture.id = LoadTextureFromFile(path.C_Str(), directory);
			texture.type = AiTexTypeToTexType(type);
			texture.path = std::string(path.C_Str());
			textures.push_back(texture);
			texture_loaded[texture.path] = texture;
		}
	}

	return textures;
}

GLuint LoadTextureFromFile(const char* path, const std::string& directory)
{
	GLuint texID;
	glGenTextures(1, &texID);

	std::string imagePath(path);
	imagePath = directory + '/' + path;

	int width, height, nrChannels;
	unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		switch (nrChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			format = GL_RED;
			break;
		}
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load image " << imagePath << std::endl;
	}
	stbi_image_free(data);

	return texID;
}

TextureType AiTexTypeToTexType(aiTextureType aiType)
{
	TextureType texType;
	switch (aiType)
	{
	case aiTextureType_NONE:
		break;
	case aiTextureType_DIFFUSE:
		texType = TextureType::DIFFUSE;
		break;
	case aiTextureType_SPECULAR:
		texType = TextureType::SPECULAR;
		break;
	default:
		texType = TextureType::DIFFUSE;
		break;
	}

	return texType;
}


