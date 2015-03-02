#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "glew.h"
#include "freeglut.h"
 
typedef struct Vertex
{
  float Position[4];
  float Color[4];
} Vertex;
 
void ExitOnGLError(const char* error_message);
GLuint LoadShader(const char* filename, GLenum shader_type);
GLuint loadBMP(const char *imagePath);
 
#endif