#ifndef MODEL_H
#define MODEL_H

#include"mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION//定义它会让预处理器修改头文件,让其只包含相关的函数定义源码.等于是把头文件变为了.cpp
#include"stb_image.h"

#include<map>

unsigned int TextureFromFile(const char*, std::string&);
 
class Model {
public:
	std::map<std::string, Texture>texture_loaded;
	Model(std::string& path) {
		LoadModel(path);
	}
	void Draw(const Shader& shader)
	{
		for (int i = 0; i < meshes.size(); i++) {
			meshes[i].Draw(shader);
		}
	}
	std::vector<Mesh>meshes;
private:
	/*模型数据*/

	std::string directory;
	

	/*函数*/
	void LoadModel(std::string& path);
	void ProcessNode(aiNode* node, const aiScene* scene);

	std::vector<Texture>LoadMaterialTextures(aiMaterial*, aiTextureType, std::string);

	Mesh ProcessMesh(aiMesh* aimesh, const aiScene* scene);

};

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture>textures;
	for (int i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		material->GetTexture(type, i, &str);
		Texture texture;
		if (texture_loaded.find(str.C_Str())!=texture_loaded.end())
		{
			textures.push_back(texture_loaded[str.C_Str()]);
		}
		else
		{
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			textures.push_back(texture);
			texture_loaded[str.C_Str()] = texture;
		}

	}
	return textures;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
	//处理该node上的每一个mesh
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];//node中获得该mesh的索引，使用索引去scene中找到正确的mesh
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	//递归处理其他子节点
	for (int i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

unsigned int TextureFromFile(const char* path, std::string& directory)
{
	std::string fileName = std::string(path);
	fileName = directory + '/' + fileName;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, heigth, nrComponents;
	unsigned char* data = stbi_load(fileName.c_str(), &width, &heigth, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, heigth, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "图片加载错误: " << fileName << std::endl;
	}
	stbi_image_free(data);
	return textureID;
}

void Model::LoadModel(std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_FlipUVs | aiProcess_Triangulate| aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "模型加载错误::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);

}

Mesh Model::ProcessMesh(aiMesh* aimesh, const aiScene* scene)
{
	//数据存储 顶点、索引、贴图
	std::vector<Vertex>vertexs;
	std::vector<unsigned int>indices;
	std::vector<Texture>textures;

	/*处理顶点数据*/
	for (int i = 0; i < aimesh->mNumVertices; i++) {
		Vertex vertex;

		/*顶点位置*/
		glm::vec3 position;
		position.x = aimesh->mVertices[i].x;
		position.y = aimesh->mVertices[i].y;
		position.z = aimesh->mVertices[i].z;
		vertex.Position = position;

		/*法线*/
		glm::vec3 normal;
		normal.x = aimesh->mNormals[i].x;
		normal.y = aimesh->mNormals[i].y;
		normal.z = aimesh->mNormals[i].z;
		vertex.Normal = normal;

		/*纹理坐标*/
	//Assimp运行一个顶点上最多有8组纹理坐标，这里只取第一组
		if (aimesh->mTextureCoords[0]) {
			glm::vec2 uv;
			uv.x = aimesh->mTextureCoords[0][i].x;//第一组,第i个顶点的纹理坐标
			uv.y = aimesh->mTextureCoords[0][i].y;
			vertex.TexCoords = uv;
		}
		else
			vertex.TexCoords = glm::vec2(0, 0);

		/*切线*/
		glm::vec3 tangent;
		tangent.x = aimesh->mTangents[i].x;
		tangent.y = aimesh->mTangents[i].y;
		tangent.z = aimesh->mTangents[i].z;
		vertex.Tangent = tangent;

		vertexs.push_back(vertex);
	}

	/*处理索引*/
		//每一个网格都有一组face数组,一个face就对应一个图元,这里就是一个三角形
	for (int i = 0; i < aimesh->mNumFaces; i++) //遍历face数组
	{
		for (int j = 0; j < aimesh->mFaces[i].mNumIndices; j++) //遍历该图元的索引坐标
		{
			indices.push_back(aimesh->mFaces[i].mIndices[j]);
		}
	}

	/*处理贴图*/
	if (aimesh->mMaterialIndex >= 0)//检测网格是否有材质
	{
		aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];

		std::vector<Texture>diffuse = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuse.begin(), diffuse.end());

		std::vector<Texture>specular = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specular.begin(), specular.end());

		std::vector<Texture>normal = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normal.begin(), normal.end());

	}

	return Mesh(vertexs, indices, textures);
}

#endif 


