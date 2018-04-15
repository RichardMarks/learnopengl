#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 480
#define WINDOW_TITLE "Learn OpenGL"

class App {
  public:
    static void onError(int errorCode, const char* msg) { throw std::runtime_error(msg); }
    static void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
      if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
      }
    }

    App();
    ~App();
    void create();
    void render();

  private:
    GLFWwindow* mainWindowPtr;
};

App::App() : mainWindowPtr(0) {
  glfwSetErrorCallback(App::onError);

  if (!glfwInit()) {
    throw std::runtime_error("glfwInit failed.");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  mainWindowPtr = glfwCreateWindow(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    WINDOW_TITLE,
    NULL,
    NULL);

  if (!mainWindowPtr) {
    throw std::runtime_error("glfwCreateWindow failed. Your hardware probably does not support OpenGL 4.1");
  }

  glfwMakeContextCurrent(mainWindowPtr);

  glfwSetKeyCallback(mainWindowPtr, App::onKey);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("glewInit failed");
  }

  std::cout << "OpenGL version supported: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version supported: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;

  if (!GLEW_VERSION_4_1) {
    throw std::runtime_error("OpenGL 4.1 API is not available.\nUnfortunately this means that you cannot play this game with your current hardware.");
  }

  int screenWidth, screenHeight;
  glfwGetFramebufferSize(mainWindowPtr, &screenWidth, &screenHeight);
  glViewport(0, 0, screenWidth, screenHeight);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  create();

  while (!glfwWindowShouldClose(mainWindowPtr)) {
    glfwPollEvents();
    render();
  }
}

App::~App() {
  glfwTerminate();
}

void App::create() {

}

void App::render() {
  glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glfwSwapBuffers(mainWindowPtr);
}

int main(int argc, char* argv[]) {
  try {
    App app;
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
