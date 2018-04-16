#include <iostream>
#include <sstream>
#include <cassert>
#include <cmath>
#include <stdexcept>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "smiley_face.h"

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

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int program;
    unsigned int texture;
};

App::App() :
  mainWindowPtr(0),
  vao(0),
  vbo(0),
  ebo(0),
  program(0) {
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
  glUseProgram(0);
  glDeleteTextures(1, &texture);
  glDeleteBuffers(1, &ebo);
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  glfwTerminate();
}

void App::create() {
  // coordinate system by default:
  // (-1.0, 1.0) is top left
  // (1.0, -1.0) is bottom right
  float vertices[] = {
    // x, y, z, r, g, b, s, t
    -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     1.0f,-1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
  };

  unsigned int elements[] = {
    0, 1, 2,
    2, 3, 0
  };

  const char* vertexShaderSource =
    "#version 410 core\n"
    "layout(location = 0) in vec3 vertex;\n"
    "layout(location = 1) in vec3 color;\n"
    "layout(location = 2) in vec2 texcoord;\n"
    "out vec3 fragColor;\n"
    "out vec2 fragTexcoord;\n"
    "void main(){\n"
    "gl_Position = vec4(vertex, 1.0);\n"
    "fragColor=color;\n"
    "fragTexcoord=texcoord;"
    "\n}";

  const char* fragmentShaderSource =
    "#version 410 core\n"
    "in vec3 fragColor;\n"
    "in vec2 fragTexcoord;\n"
    "out vec4 color;\n"
    "uniform sampler2D tex;\n"
    "void main(){\n"
    "color = texture(tex, fragTexcoord) * vec4(fragColor, 1);}";

  unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

  int vsCompileStatus = 0;
  int fsCompileStatus = 0;

  glShaderSource(vs, 1, &vertexShaderSource, 0);
  glCompileShader(vs);

  glGetShaderiv(vs, GL_COMPILE_STATUS, &vsCompileStatus);
  if (vsCompileStatus == GL_FALSE) {
    throw std::runtime_error("Vertex Shader Compilation Failed");
  }

  glShaderSource(fs, 1, &fragmentShaderSource, 0);
  glCompileShader(fs);

  glGetShaderiv(fs, GL_COMPILE_STATUS, &fsCompileStatus);
  if (fsCompileStatus == GL_FALSE) {
    throw std::runtime_error("Fragment Shader Compilation Failed");
  }

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);

  int linkStatus = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) {
    throw std::runtime_error("Shader Program Link Failed");
  }

  glDetachShader(program, vs);
  glDetachShader(program, fs);

  glDeleteShader(vs);
  glDeleteShader(fs);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  glUseProgram(program);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  int textureWidth = smileyFaceWidth;
  int textureHeight = smileyFaceHeight;
  float* pixels = smileyFacePixels;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_FLOAT, pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void App::render() {
  glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glfwSwapBuffers(mainWindowPtr);

  glUseProgram(0);

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
