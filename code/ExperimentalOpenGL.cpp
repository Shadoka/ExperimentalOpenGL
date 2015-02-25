#include "incl/utils.h"
#include "utils.c"
#define WINDOW_TITLE_PREFIX "Chapter 2"

typedef struct {
  float XYZW[4];
  float RGBA[4];
} vertex;

int currentWidth = 600,
    currentHeight = 600,
    WindowHandle = 0;

unsigned frameCount = 0;

GLuint vertexShaderId,
       fragmentShaderId,
       programId,
       vaoId,
       vboId,
       indexBufferId[2],
       activeIndexBuffer = 0,
       vertsToRender = 48;
 
void initialize(int, char*[]);
void initWindow(int, char*[]);
void resizeFunction(int, int);
void renderFunction(void);
void timerFunction(int);
void idleFunction(void);
void cleanup(void);
void createVBO(void);
void destroyVBO(void);
void createShaders(void);
void destroyShaders(void);
void keyboardFunction(unsigned char, int, int);
 
int main(int argc, char* argv[])
{
  initialize(argc, argv);
 
  glutMainLoop();
  
  exit(EXIT_SUCCESS);
}
 
void initialize(int argc, char* argv[])
{
  GLenum glewInitResult;

  initWindow(argc, argv);
  
  glewExperimental = GL_TRUE;
  glewInitResult = glewInit();

  if (glewInitResult != GLEW_OK) {
    fprintf(stderr, "ERROR: %s\n", glewGetErrorString(glewInitResult));
    exit(EXIT_FAILURE);
  }

  fprintf(
    stdout,
    "INFO: OpenGL Version: %s\n",
    glGetString(GL_VERSION)
  );
 
  createShaders();
  createVBO();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}
 
void initWindow(int argc, char* argv[])
{
  glutInit(&argc, argv);
  
  glutInitContextVersion(4, 0);
  glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_CORE_PROFILE);
 
  glutSetOption(
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_ACTION_GLUTMAINLOOP_RETURNS
  );
  
  glutInitWindowSize(currentWidth, currentHeight);
 
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
 
  WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);
 
  if(WindowHandle < 1) {
    fprintf(
      stderr,
      "ERROR: Could not create a new rendering window.\n"
    );
    exit(EXIT_FAILURE);
  }
 
  glutReshapeFunc(resizeFunction);
  glutDisplayFunc(renderFunction);
  glutIdleFunc(idleFunction);
  glutTimerFunc(0, timerFunction, 0);
  glutCloseFunc(cleanup);
  glutKeyboardFunc(keyboardFunction);
}

void keyboardFunction(unsigned char key, int x, int y) {
  switch(key) {
    case 'T':
    case 't': {
      activeIndexBuffer = (activeIndexBuffer == 1 ? 0 : 1);
      vertsToRender = (vertsToRender == 48 ? 36 : 48);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId[activeIndexBuffer]);
      break;
    }
    default:
      break;
  }
}
 
void resizeFunction(int Width, int Height)
{
  currentWidth = Width;
  currentHeight = Height;
  glViewport(0, 0, currentWidth, currentHeight);
}
 
void renderFunction(void)
{
  ++frameCount;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawElements(GL_TRIANGLES, vertsToRender, GL_UNSIGNED_BYTE, (GLvoid *)0);

  glutSwapBuffers();
  glutPostRedisplay();
}

void idleFunction(void) {
  glutPostRedisplay();
}

void timerFunction(int value) {
  if (value != 0) {
    char *tempString = (char *) malloc(512 + strlen(WINDOW_TITLE_PREFIX));
    sprintf(tempString, "%s: %d Frames Per Second @ %d x %d", WINDOW_TITLE_PREFIX, frameCount * 4, currentWidth, currentHeight);
    glutSetWindowTitle(tempString);
    free(tempString);
  }
  frameCount = 0;
  glutTimerFunc(250, timerFunction, 1);
}

void cleanup(void) {
  destroyShaders();
  destroyVBO();
}

void createVBO(void) {
  vertex vertices[] = {
    { { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } },
    // Top
    { { -0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { 0.2f, 0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
    { { 0.0f, 0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
    { { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    // Bottom
    { { -0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
    { { 0.2f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { 0.0f, -0.8f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
    { { 0.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    // Left
    { { -0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { -0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
    { { -0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
    { { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    // Right
    { { 0.8f, -0.2f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
    { { 0.8f, 0.2f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { 0.8f, 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
    { { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } }
  };
  GLubyte indices[] = {
    // Top
    0, 1, 3,
    0, 3, 2,
    3, 1, 4,
    3, 4, 2,
    // Bottom
    0, 5, 7,
    0, 7, 6,
    7, 5, 8,
    7, 8, 6,
    // Left
    0, 9, 11,
    0, 11, 10,
    11, 9, 12,
    11, 12, 10,
    // Right
    0, 13, 15,
    0, 15, 14,
    15, 13, 16,
    15, 16, 14
  };
  GLubyte alternateIndices[] = {
    // Outer square border:
    3, 4, 16,
    3, 15, 16,
    15, 16, 8,
    15, 7, 8,
    7, 8, 12,
    7, 11, 12,
    11, 12, 4,
    11, 3, 4,
 
    // Inner square
    0, 11, 3,
    0, 3, 15,
    0, 15, 7,
    0, 7, 11
  };

  GLenum errorCheckValue = glGetError();
  const size_t bufferSize = sizeof(vertices);
  const size_t vertexSize = sizeof(vertices[0]);
  const size_t rgbOffset = sizeof(vertices[0].XYZW);

  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);

  glGenBuffers(1, &vboId);
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, vertexSize, 0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid *)rgbOffset);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glGenBuffers(2, indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(alternateIndices), alternateIndices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId[0]);

  errorCheckValue = glGetError();
  if (errorCheckValue != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: Could not create a VBO: %s\n", gluErrorString(errorCheckValue));
    //exit(-1);
  }
}

void destroyVBO(void) {
  GLenum errorCheckValue = glGetError();

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &vboId);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(2, indexBufferId);

  glBindVertexArray(0);
  glDeleteVertexArrays(1, &vaoId);

  errorCheckValue = glGetError();
  if (errorCheckValue != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: Could not destroy the VBO: %s\n", gluErrorString(errorCheckValue));
    //exit(-1);
  }
}

void createShaders(void) {
  GLenum errorCheckValue = glGetError();

  vertexShaderId = LoadShader("vertexshader.glsl", GL_VERTEX_SHADER);

  fragmentShaderId = LoadShader("fragmentshader.glsl", GL_FRAGMENT_SHADER);

  programId = glCreateProgram();
  glAttachShader(programId, vertexShaderId);
  glAttachShader(programId, fragmentShaderId);
  glLinkProgram(programId);
  glUseProgram(programId);
  errorCheckValue = glGetError();
  if (errorCheckValue != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: Could not create the shaders: %s\n", gluErrorString(errorCheckValue));
    //exit(-1);
  }
}

void destroyShaders(void) {
  GLenum errorCheckValue = glGetError();

  glUseProgram(0);

  glDetachShader(programId, vertexShaderId);
  glDetachShader(programId, fragmentShaderId);

  glDeleteShader(fragmentShaderId);
  glDeleteShader(vertexShaderId);

  glDeleteProgram(programId);

  errorCheckValue = glGetError();
  if (errorCheckValue != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: Could not destroy shaders: %s\n", gluErrorString(errorCheckValue));
    //exit(-1);
  }
}