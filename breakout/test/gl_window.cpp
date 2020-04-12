#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

// The Width of the screen
const GLuint SCREEN_WIDTH = 800;
// The height of the screen
const GLuint SCREEN_HEIGHT = 600;

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

int Render(){
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  std::ifstream 
}

int RunGame(){
  fprintf(stdout, "INFO: start game\n");
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout", nullptr, nullptr);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  glewInit(); 
  glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

  // glfwSetKeyCallback(window, key_callback);

  // OpenGL configuration
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Initialize game
  // Breakout.Init();

  // DeltaTime variables
  GLfloat deltaTime = 0.0f;
  GLfloat lastFrame = 0.0f;

  // Start Game within Menu State
  // Breakout.State = GAME_ACTIVE;

  while (!glfwWindowShouldClose(window))
  {
    // Calculate delta time
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glfwPollEvents();

    //deltaTime = 0.001f;
    // Manage user input
    // Breakout.ProcessInput(deltaTime);

    // Update Game state
    // Breakout.Update(deltaTime);

    // Render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Breakout.Render();
    Render();

    glfwSwapBuffers(window);
  }

  // Delete all resources as loaded using the resource manager
  // ResourceManager::Clear();

  glfwTerminate();
  return 0;
}

int main(int argc, char *argv[])
{
  RunGame();
  return 0;
}
