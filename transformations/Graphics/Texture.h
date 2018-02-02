#ifndef TEXTURE_H
#define TEXTURE_H


#include <GL/glew.h>
class Texture2D
{
public:
	GLuint ID;
	GLuint Width, Height;
	//Format
	GLuint Internal_Format; // format of texture object
	GLuint Image_Format; //format of loaded image
	//Texture configuration
	GLuint Wrap_S; //Wrapping mode S axis
	GLuint Wrap_T; //Wrapping mode T axis
	GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
	GLuint Filter_Max; // Filtering mode of texture pixels > screen pixels

	Texture2D();
	void Generate(GLuint width, GLuint height, unsigned char* data);
	void Bind() const;
private:
};




#endif