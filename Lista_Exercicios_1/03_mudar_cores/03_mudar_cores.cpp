/*
Introdução a OpenGL, Buffers e Shaders

3. Repita o exercício anterior para alterar as cores dos vértices do triângulo usando teclas.
*/

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <cmath>

#include "gl_utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static int width = 800, height = 600;
float dx, dy = 0.0f;
float greenValue, redValue, blueValue = 1.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void reshape_callback(GLFWwindow* window, int width, int height);
void showFPS(GLFWwindow* window);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    static bool gWireframe = 0;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        if (dx >= -0.4f)
            dx -= 0.1;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        if (dx <= 0.4f)
            dx += 0.1;
    }
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        if (dy <= 0.4f)
            dy += 0.1;
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        if (dy >= -0.4f)
            dy -= 0.1;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        redValue = 1.0f;
        greenValue = 0.0f;
        blueValue = 0.0f;
    }
    else if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        redValue = 0.0f;
        greenValue = 1.0f;
        blueValue = 0.0f;
    }
    else if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        redValue = 0.0f;
        greenValue = 0.0f;
        blueValue = 1.0f;
    }
}

void reshape_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void showFPS(GLFWwindow* window)
{
    static double previousSeconds = 0.0;
    static int frameCount = 0;
    double elapsedSeconds;
    double currentSeconds = glfwGetTime();

    elapsedSeconds = currentSeconds - previousSeconds;

    if (elapsedSeconds > 0.25)
    {
        previousSeconds = currentSeconds;
        double fps = (double)frameCount / elapsedSeconds;
        double msPerFrame = 1000.0 / fps;

        std::ostringstream outs;
        outs.precision(3);
        outs << std::fixed
            << "Shaders:: uniforms" << "    "
            << "FPS: " << fps << "    "
            << "Frame Time: " << msPerFrame << " (ms)";
        glfwSetWindowTitle(window, outs.str().c_str());

        frameCount = 0;
    }

    frameCount++;
}

int main()
{

    GLFWwindow* g_window;

    if (!glfwInit())
        return -1;
#ifdef APPLE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    g_window = glfwCreateWindow(width, height, "Hello Triangle Indexed", NULL, NULL);
    if (!g_window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(g_window);
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(g_window, &screenWidth, &screenHeight);
    glfwSetFramebufferSizeCallback(g_window, reshape_callback);
    glfwSetKeyCallback(g_window, key_callback);

    if (gl3wInit()) {
        std::cout << "failed to initialize OpenGL\n" << std::endl;
        return -1;
    }

    if (!gl3wIsSupported(3, 2)) {
        std::cout << "OpenGL 3.2 not supported\n" << std::endl;
        return -1;
    }

    char vertex_shader[1024 * 256];
    char fragment_shader[1024 * 256];
    parse_file_into_str("vs_uniform.glsl", vertex_shader, 1024 * 256);
    parse_file_into_str("fs_uniform.glsl", fragment_shader, 1024 * 256);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* p = (const GLchar*)vertex_shader;
    glShaderSource(vs, 1, &p, NULL);
    glCompileShader(vs);

    GLint result;
    GLchar infoLog[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
        std::cout << "Error! Vertex shader failed to compile. " << infoLog << std::endl;
    }

    GLint fs = glCreateShader(GL_FRAGMENT_SHADER);
    p = (const GLchar*)fragment_shader;
    glShaderSource(fs, 1, &p, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
        std::cout << "Error! Fragment shader failed to compile. " << infoLog << std::endl;
    }

    GLint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    if (!result)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        std::cout << "Error! Shader program linker failure. " << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLfloat vertices[] = {
      0.0f,  0.5f, 0.0f,		// Top
      0.5f, -0.5f, 0.0f,		// Bottom right
      -0.5f, -0.5f, 0.0f		// Bottom left
    };

    GLuint vbo, vao;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glUseProgram(shaderProgram);

    glm::mat4 model_cpu(1.0f);
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");

    // Rendering loop
    while (!glfwWindowShouldClose(g_window))
    {
        model_cpu = glm::translate(glm::mat4(1.0f), glm::vec3(dx, dy, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_cpu));

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

        glUniform4f(vertexColorLocation, redValue, greenValue, blueValue, 1.0f);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(g_window);
    }

    glDeleteProgram(shaderProgram);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();

    return 0;
}
