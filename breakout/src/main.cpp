

/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "resource_manager.h"

// for test 
#include <fstream>
#include <sstream>

// GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const GLuint SCREEN_WIDTH = 800;
// The height of the screen
const GLuint SCREEN_HEIGHT = 600;

Game Breakout(SCREEN_WIDTH, SCREEN_HEIGHT);

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

  glfwSetKeyCallback(window, key_callback);

  // OpenGL configuration
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Initialize game
  Breakout.Init();

  // DeltaTime variables
  GLfloat deltaTime = 0.0f;
  GLfloat lastFrame = 0.0f;

  // Start Game within Menu State
  Breakout.State = GAME_ACTIVE;

  while (!glfwWindowShouldClose(window))
  {
    // Calculate delta time
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glfwPollEvents();

    //deltaTime = 0.001f;
    // Manage user input
    Breakout.ProcessInput(deltaTime);

    // Update Game state
    Breakout.Update(deltaTime);

    // Render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    Breakout.Render();

    glfwSwapBuffers(window);
  }

  // Delete all resources as loaded using the resource manager
  ResourceManager::Clear();

  glfwTerminate();
  return 0;
}

int main(int argc, char *argv[])
{

  // read file 
  // // auto vsfile = "../src/shaders/sprite.vert";
  // auto vsfile = "../src/textures/awesomeface.png";
  // std::ifstream vertexShaderFile(vsfile);
  // std::stringstream vShaderStream;
  // vShaderStream << vertexShaderFile.rdbuf();

  // fprintf(stdout, "rebuf %s\n", vShaderStream.str().c_str());
  // vertexShaderFile.close();
  // return 0;

  RunGame();
  return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Breakout.Keys[key] = GL_TRUE;
        else if (action == GLFW_RELEASE)
            Breakout.Keys[key] = GL_FALSE;
    }
}

