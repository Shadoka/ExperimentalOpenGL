#include "incl/Utils.h"
 
void ExitOnGLError(const char* error_message)
{
  const GLenum ErrorValue = glGetError();
 
  if (ErrorValue != GL_NO_ERROR)
  {
    fprintf(stderr, "%s: %s\n", error_message, gluErrorString(ErrorValue));
  }
}
 
GLuint LoadShader(const char* filename, GLenum shader_type)
{
  GLuint shader_id = 0;
  FILE* file;
  long file_size = -1;
  char* glsl_source;
 
  if (NULL != (file = fopen(filename, "rb")) &&
    0 == fseek(file, 0, SEEK_END) &&
    -1 != (file_size = ftell(file)))
  {
    rewind(file);
    
    if (NULL != (glsl_source = (char*)malloc(file_size + 1)))
    {
      if (file_size == (long)fread(glsl_source, sizeof(char), file_size, file))
      {
        glsl_source[file_size] = '\0';
 
        if (0 != (shader_id = glCreateShader(shader_type)))
        {
          glShaderSource(shader_id, 1, &glsl_source, NULL);
          glCompileShader(shader_id);
          ExitOnGLError("Could not compile a shader");
        }
        else
          fprintf(stderr, "ERROR: Could not create a shader.\n");
      }
      else
        fprintf(stderr, "ERROR: Could not read file %s\n", filename);
 
      free(glsl_source);
    }
    else
      fprintf(stderr, "ERROR: Could not allocate %i bytes.\n", file_size);
 
    fclose(file);
  }
  else
  {
    if (NULL != file)
      fclose(file);
    fprintf(stderr, "ERROR: Could not open file %s\n", filename);
  }
 
  return shader_id;
}

GLuint loadBMP(const char *imagePath) {
  unsigned char header[54];
  unsigned int dataPos;
  unsigned int width, height;
  unsigned int imageSize;
  unsigned char *data;

  FILE *file = fopen(imagePath, "rb");
  if (!file) {
    fprintf(stderr, "Could not open BMP: %s\n", imagePath);
    return 0;
  }
  if (fread(header, 1, 54, file) != 54) {
    fprintf(stderr, "Not a correct BMP file: %s\n", imagePath);
    return 0;
  }
  if (header[0] != 'B' || header[1] != 'M') {
    fprintf(stderr, "Not a correct BMP file: %s\n", imagePath);
    return 0;
  }
  dataPos    = *(int*)&(header[0x0A]);
  imageSize  = *(int*)&(header[0x22]);
  width      = *(int*)&(header[0x12]);
  height     = *(int*)&(header[0x16]);
  if (imageSize==0) imageSize = width*height*3;
  if (dataPos==0) dataPos = 54;
  data = new unsigned char[imageSize];
  fread(data, 1, imageSize, file);
  fclose(file);

  GLuint textureId;
  glGenTextures(1, &textureId);

  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  return textureId;
}