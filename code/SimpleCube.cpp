// windows.h wird für XInput benötigt. blargh.
#include <windows.h>
#include <XInput.h>
#include "utils.c"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define WINDOW_TITLE_PREFIX "Chapter 2"

int currentWidth = 600,
    currentHeight = 600,
    WindowHandle = 0;

unsigned frameCount = 0;

GLuint bufferIds[3] = {0},
  shaderIds[3] = {0},
  textureId;

GLint uniform_mvp;
float stickX = 0.0f;
float stickY = 0.0f;

void initialize(int, char*[]);
void initWindow(int, char*[]);
void resizeFunction(int, int);
void renderFunction(void);
void timerFunction(int);
void idleFunction(void);
void drawCube(void);
void createCube(void);
void destroyCube(void);
void evalController(void);
 
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
}
 
void resizeFunction(int Width, int Height)
{
  fprintf(stdout, "resize\n");
  currentWidth = Width;
  currentHeight = Height;
  glViewport(0, 0, currentWidth, currentHeight);
}
 
void renderFunction(void)
{
  ++frameCount;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  evalController();
  drawCube();

  glutSwapBuffers();
}

void evalController() {
  for (int x = 0; x < 4; x++) {
    XINPUT_STATE inputState;
    if (XInputGetState(x, &inputState) == ERROR_SUCCESS) {
      XINPUT_GAMEPAD *pad = &inputState.Gamepad;

	  float deadzoneX = 0.5f;
	  float deadzoneY = 0.3f;

	  float normLX = fmaxf(-1, (float)pad->sThumbLX / 32767);
	  float normLY = fmaxf(-1, (float)pad->sThumbLY / 32767);

	  stickX = (abs(normLX) < deadzoneX ? 0 : (abs(normLX) - deadzoneX) * (normLX / abs(normLX)));
	  stickY = (abs(normLY) < deadzoneY ? 0 : (abs(normLY) - deadzoneY) * (normLY / abs(normLY)));

	  if (deadzoneX > 0) stickX *= 1 / (1 - deadzoneX);
	  if (deadzoneY > 0) stickY *= 1 / (1 - deadzoneY);
	}
  }
}

void createCube(void) {
  const Vertex vertices[8] = {
    {{-0.5f, -0.5f,  0.5f, 1}, {0, 0, 1, 1}},
    {{-0.5f,  0.5f,  0.5f, 1}, {1, 0, 0, 1}},
    {{ 0.5f,  0.5f,  0.5f, 1}, {0, 1, 0, 1}},
    {{ 0.5f, -0.5f,  0.5f, 1}, {1, 1, 0, 1}},
    {{-0.5f, -0.5f, -0.5f, 1}, {1, 1, 1, 1}},
    {{-0.5f,  0.5f, -0.5f, 1}, {1, 0, 0, 1}},
    {{ 0.5f,  0.5f, -0.5f, 1}, {1, 0, 1, 1}},
    {{ 0.5f, -0.5f, -0.5f, 1}, {0, 0, 1, 1}}
  };
  const GLuint indices[36] = {
    0, 2, 1,	0, 3, 2,
    4, 3, 0,	4, 7, 3,
    4, 1, 5,	4, 0, 1,
    3, 6, 2,	3, 7, 6,
    1, 6, 5,	1, 2, 6,
    7, 5, 6,	7, 4, 5
  };
  shaderIds[0] = glCreateProgram();
  ExitOnGLError("Error at creating the shader program");

  shaderIds[1] = LoadShader("fragmentshader_cube.glsl", GL_FRAGMENT_SHADER);
  shaderIds[2] = LoadShader("vertexshader_cube.glsl", GL_VERTEX_SHADER);
  glAttachShader(shaderIds[0], shaderIds[1]);
  glAttachShader(shaderIds[0], shaderIds[2]);
  glLinkProgram(shaderIds[0]);
  ExitOnGLError("Error at linking shader program");

  uniform_mvp = glGetUniformLocation(shaderIds[0], "mvp");
  if (uniform_mvp == -1) {
    fprintf(stdout, "uniform nicht gefunden\n");
  }

  textureId = loadBMP("texture.bmp");
  ExitOnGLError("ERROR: Could not create textures.");

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
 
  glDeleteTextures(1, &textureId);
  glDeleteBuffers(2, &bufferIds[1]);
  glDeleteVertexArrays(1, &bufferIds[0]);
  ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

void drawCube(void) {
  float angleX = (glutGet(GLUT_ELAPSED_TIME) / 10000.0) * stickX * 5;
  float angleY = (glutGet(GLUT_ELAPSED_TIME) / 10000.0) * stickY * 5;
  fprintf(stdout, "%f\n", angleX);
  glm::vec3 axis_y(0.0, 1.0, 0.0);
  glm::vec3 axis_x(1.0, 0.0, 0.0);
  glm::mat4 animX = glm::rotate(glm::mat4(1.0f), angleX, axis_y);
  glm::mat4 animY = glm::rotate(glm::mat4(1.0f), angleY, axis_x);
  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
  glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 projection = glm::perspective(45.0f, 1.0f*currentWidth/currentHeight, 0.1f, 10.0f);
  glm::mat4 mvp = projection * view * model * animX * animY;

  glUseProgram(shaderIds[0]);
  ExitOnGLError("ERROR: Could not use the shader program");
 
  glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

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