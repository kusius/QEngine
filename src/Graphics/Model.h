#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <iostream>

#include "Thirdparty/assimp/Importer.hpp"
#include "Thirdparty/assimp/scene.h"
#include "Thirdparty/assimp/postprocess.h"

#include "Shader.h"
#include "Mesh.h"

using namespace std;

class Model
{
public:
	Model(char *path)
	{
		loadModel(path);
	}
	void Draw(Shader shader);

private:
	vector<Mesh> meshes;
	vector<Texture> textures_loaded;
	string directory;

	void loadModel(string path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
										 string typeName);
	unsigned int loadTextureFromFile(const char *file, const string &directory);
};
#endif // !MODEL_H
