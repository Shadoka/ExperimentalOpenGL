#include "incl/utils.h"
#include "utils.c"
#define WINDOW_TITLE_PREFIX "Chapter 2"

int currentWidth = 600,
    currentHeight = 600,
    WindowHandle = 0;

unsigned frameCount = 0;

GLuint projectionMatrixUniformLocation,
  viewMatrixUniformLocation,
  modelMatrixUniformLocation,
  bufferIds[3] = {0},
  shaderIds[3] = {0};

Matrix projectionMatrix,
  viewMatrix,
  modelMatrix;

float cubeRotation = 0;
clock_t lastTime;
 
void initialize(int, char*[]);
void initWindow(int, char*[]);
void resizeFunction(int, int);
void renderFunction(void);
void timerFunction(int);
void idleFunction(void);
void drawCube(void);
void createCube(void);
void destroyCube(void);
 
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
 
  glGetError();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  ExitOnGLError("Error at creating depth testing options");

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  ExitOnGLError("Error at creating culling options");

  modelMatrix = IDENTITY_MATRIX;
  projectionMatrix = IDENTITY_MATRIX;
  viewMatrix = IDENTITY_MATRIX;
  TranslateMatrix(&viewMatrix, 0, 0, -2);

  createCube();
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
  glutCloseFunc(destroyCube);
  //glutKeyboardFunc(keyboardFunction);
}
 
void resizeFunction(int Width, int Height)
{
  currentWidth = Width;
  currentHeight = Height;
  glViewport(0, 0, currentWidth, currentHeight);
  projectionMatrix = CreateProjectionMatrix(60, (float) currentWidth/currentHeight, 1.0f, 100.0f);
  glUseProgram(shaderIds[0]);
  glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, projectionMatrix.m);
  glUseProgram(0);
}
 
void renderFunction(void)
{
  ++frameCount;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawCube();

  glutSwapBuffers();
}

void createCube(void) {
  const Vertex vertices[8] = {
    {{-.5f, -.5f,  .5f, 1 }, {0, 0, 1, 1 }},
    {{-.5f,  .5f,  .5f, 1 }, {1, 0, 0, 1 }},
    {{ .5f,  .5f,  .5f, 1 }, {0, 1, 0, 1 }},
    {{ .5f, -.5f,  .5f, 1 }, {1, 1, 0, 1 }},
    {{-.5f, -.5f, -.5f, 1 }, {1, 1, 1, 1 }},
    {{-.5f,  .5f, -.5f, 1 }, {1, 0, 0, 1 }},
    {{ .5f,  .5f, -.5f, 1 }, {1, 0, 1, 1 }},
    {{ .5f, -.5f, -.5f, 1 }, {0, 0, 1, 1 }}
  };
  const GLuint indices[36] = {
    0,2,1,  0,3,2,
    4,3,0,  4,7,3,
    4,1,5,  4,0,1,
    3,6,2,  3,7,6,
    1,6,5,  1,2,6,
    7,5,6,  7,4,5
  };
  shaderIds[0] = glCreateProgram();
  ExitOnGLError("Error at creating the shader program");

  shaderIds[1] = LoadShader("fragmentshader.glsl", GL_FRAGMENT_SHADER);
  shaderIds[2] = LoadShader("vertexshader.glsl", GL_VERTEX_SHADER);
  glAttachShader(shaderIds[0], shaderIds[1]);
  glAttachShader(shaderIds[0], shaderIds[2]);
  glLinkProgram(shaderIds[0]);
  ExitOnGLError("Error at linking shader program");

  modelMatrixUniformLocation = glGetUniformLocation(shaderIds[0], "modelMatrix");
  viewMatrixUniformLocation = glGetUniformLocation(shaderIds[0], "viewMatrix");
  projectionMatrixUniformLocation = glGetUniformLocation(shaderIds[0], "projectionMatrix");
  ExitOnGLError("Error at getting the shader uniform locations");

  glGenVertexArrays(1, &bufferIds[0]);
  ExitOnGLError("ERROR: Could not generate the VAO");
  glBindVertexArray(bufferIds[0]);
  ExitOnGLError("ERROR: Could not bind the VAO");

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  ExitOnGLError("ERROR: Could not enable vertex attributes");

  glGenBuffers(2, &bufferIds[1]);
  ExitOnGLError("ERROR: Could not generate the buffer objects");

  glBindBuffer(GL_ARRAY_BUFFER, bufferIds[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  ExitOnGLError("ERROR: Could not bind the VBO to the VAO");
 
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)0);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (GLvoid*)sizeof(vertices[0].Position));
  ExitOnGLError("ERROR: Could not set VAO attributes");

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIds[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  ExitOnGLError("ERROR: Could not bind the IBO to the VAO");
 
  glBindVertexArray(0);
}

void destroyCube(void) {
  glDetachShader(shaderIds[0], shaderIds[1]);
  glDetachShader(shaderIds[0], shaderIds[2]);
  glDeleteShader(shaderIds[1]);
  glDeleteShader(shaderIds[2]);
  glDeleteProgram(shaderIds[0]);
  ExitOnGLError("ERROR: Could not destroy the shaders");
 
  glDeleteBuffers(2, &bufferIds[1]);
  glDeleteVertexArrays(1, &bufferIds[0]);
  ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

void drawCube(void) {
  float cubeAngle;
  clock_t now = clock();
  if (lastTime == 0) {
    lastTime = now;
  }
  cubeRotation += 45.0f * ((float)(now - lastTime) / CLOCKS_PER_SEC);
  cubeAngle = DegreesToRadians(cubeRotation);
  lastTime = now;

  modelMatrix = IDENTITY_MATRIX;
  RotateAboutY(&modelMatrix, cubeAngle);
  RotateAboutX(&modelMatrix, cubeAngle);

  glUseProgram(shaderIds[0]);
  ExitOnGLError("ERROR: Could not use the shader program");
 
  glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, modelMatrix.m);
  glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_FALSE, viewMatrix.m);
  glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, projectionMatrix.m);
  ExitOnGLError("ERROR: Could not set the shader uniforms");

  glBindVertexArray(bufferIds[0]);
  ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");
 
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)0);
  ExitOnGLError("ERROR: Could not draw the cube");
 
  glBindVertexArray(0);
  glUseProgram(0);
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