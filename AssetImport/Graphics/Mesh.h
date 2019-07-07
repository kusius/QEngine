#ifndef MESH_H
#define MESH_H

//A 3D mesh storage and rednering class

#include <string>
#include <vector>

#include <GLM/glm.hpp>
#include <glad/glad.h>

#include "Shader.h"

using namespace std;

struct Vertex 
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};


struct Texture
{
	unsigned int id;
	string  type;
	string path;
};

class Mesh
{

public:
	/*Mesh Data*/
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	/*Functions*/
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw(Shader shader);

private:
	/*Render data*/
	unsigned int VAO, VBO, EBO;
	/*Functions*/
	void setupMesh();


};



#endif 

