#include "ModuleModelLoader.h"
#include "Application.h"
#include "ModuleShader.h"
#include "ModuleTexture.h"

#include <cimport.h>
#include <postprocess.h>
#include <scene.h>
#include <material.h>
#include <mesh.h>

ModuleModelLoader::ModuleModelLoader() {
}
ModuleModelLoader::~ModuleModelLoader() {
}

void ModuleModelLoader::Draw()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw();
}

bool ModuleModelLoader::Init() {
	LOG("Init Model Loader\n");
	Load("baker/BakerHouse.fbx");
	return true;
}

bool ModuleModelLoader::CleanUp() {

	return true;
}

void ModuleModelLoader::SwitchModel(const char *file)
{
	if (model) {
		textures_loaded.clear();
		meshes.clear();
		directory.clear();
		model = false;
	}
	LOG("\nNew model added. Loading Model %s \n", file);
	Load(file);
}

void ModuleModelLoader::Load(const char* path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		LOG( "ERROR::ASSIMP:: %s \n",importer.GetErrorString());
		return;
	}
	directory = GetModelDirectory(path);
	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
	
	model = true;
}

void ModuleModelLoader::processNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh ModuleModelLoader::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		vertex.Position = float3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.TexCoords = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		
		//LOG("Texture %d coord x %f coord y %f \n", i, mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		vertices.push_back(vertex);
	}

	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

vector<Texture> ModuleModelLoader::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			//std::string newPath = std::string("dragon2/") + std::string(str.C_Str());
			//const char* pathing = newPath.c_str();
			std::string newPath = directory.c_str() + string(str.C_Str());
			const char* pathing = newPath.c_str();
			texture.id = App->texture->Load((char*)pathing);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}//
	return textures;
}
string ModuleModelLoader::GetModelDirectory(const char *path) 
{
	std::string dir = std::string(path);

	std::size_t currentDir = dir.find_last_of("/\\");
	std::string modelDir = dir.substr(0, currentDir + 1);

	return modelDir;
}