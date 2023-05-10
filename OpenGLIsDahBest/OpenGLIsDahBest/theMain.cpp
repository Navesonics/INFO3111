//#include <glad/glad.h>
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include "globalStuff.h"

#include <iostream>
//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>

#include "cShaderManager/cShaderManager.h"



//static const struct
//{
//    float x, y;
//    float r, g, b;
//} vertices[3] =
//{
//    { -0.6f, -0.4f, 1.f, 0.f, 0.f },            // Spawn a vertex shader instance
//    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
//    {   0.f,  0.6f, 0.f, 0.f, 1.f }
//};

struct sVertexXYZ_RGB
{
    float x, y, z;      // vec2 to vec3 
    float r, g, b;
};

// Google C dynamic array
sVertexXYZ_RGB* pSexyVertex = new sVertexXYZ_RGB[11582 * 3];

sVertexXYZ_RGB vertices[3] =
{
    { -6.6f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f },            // Spawn a vertex shader instance
    {  6.6f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f },
    {  6.0f,  0.6f, 0.0f, 1.0f, 0.0f, 0.0f }
};

//static const char* vertex_shader_text =
//"#version 110\n"
//"uniform mat4 MVP;\n"
//"attribute vec3 vCol;\n"
//"attribute vec2 vPos;\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_Position = MVP * vec4(vPos.x, vPos.y, 0.0, 1.0);\n"
//"    color = vCol;\n"
//"}\n";

//static const char* fragment_shader_text =
//"#version 110\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_FragColor = vec4(color, 1.0);\n"
//"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer;
//    GLuint vertex_shader;
//    GLuint fragment_shader;
//    GLuint program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);

    if ( ! glfwInit() )
    {
//        exit(EXIT_FAILURE);
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);

    if ( ! window )
    {
        glfwTerminate();
//        exit(EXIT_FAILURE);
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress );
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    unsigned int numberOfBytes = sizeof(sVertexXYZ_RGB) * 3;
    
    glBufferData(GL_ARRAY_BUFFER, 
                 numberOfBytes,    // Each vertex in bytes
                 vertices,                  // Pointer to the start of the array
                 GL_STATIC_DRAW);

//
//    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
//    glCompileShader(vertex_shader);
//
//    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
//    glCompileShader(fragment_shader);
//
//    program = glCreateProgram();
//    glAttachShader(program, vertex_shader);
//    glAttachShader(program, fragment_shader);
//    glLinkProgram(program);
// 
    cShaderManager* pShaderManager = new cShaderManager();

    cShaderManager::cShader vertexShader;
    cShaderManager::cShader fragementShader;

    pShaderManager->setBasePath("assets/shaders/");

    vertexShader.fileName = "vertexShader01.glsl";
    fragementShader.fileName = "fragmentShader01.glsl";

   if ( !pShaderManager->createProgramFromFile( "basicShader",
                                                 vertexShader, fragementShader ) )
   {
       std::cout << "Error: Can't create shader program:" << std::endl;
       std::cout << pShaderManager->getLastError() << std::endl;
   }

//   glUseProgram(program);
    GLuint shaderProgram = pShaderManager->getIDFromFriendlyName("basicShader");
    glUseProgram(shaderProgram);

    mvp_location = glGetUniformLocation(shaderProgram, "MVP");


// Vertex layout specification
 //   struct sVertexXYZ_RGB {
//        float x, y, z;      // vec2 to vec3 
//        float r, g, b;  };
    vpos_location = glGetAttribLocation(shaderProgram, "vPos");
    vcol_location = glGetAttribLocation(shaderProgram, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 
                          3, 
                          GL_FLOAT, 
                          GL_FALSE,
                          sizeof(sVertexXYZ_RGB),       // sizeof(vertices[0]), 
                          (void*)offsetof(sVertexXYZ_RGB, x)); // (void*)0);

    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 
                          3, 
                          GL_FLOAT, 
                          GL_FALSE,
                          sizeof(sVertexXYZ_RGB),   // sizeof(vertices[0]), 
                          (void*)offsetof(sVertexXYZ_RGB, r));
                          //(void*)(sizeof(float) * 3));
//


    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
//        mat4x4 m, p, mvp;
        glm::mat4 m, p, v, mvp;


        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

//        mat4x4_identity(m);
        m = glm::mat4(1.0f);        // Identity matrix
//        mat4x4_rotate_Z(m, m, (float)glfwGetTime());
        glm::mat4 matRotateZ = glm::rotate(glm::mat4(1.0f),
                                           0.0f, ///(float)glfwGetTime(),
                                           glm::vec3(0.0f, 1.0f, 0.0f));
//        mat4x4_mul(mvp, p, m);
        m = matRotateZ * m;


//        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

        p = glm::perspective(0.6f,
                             ratio,
                             0.1f,          // Near plane
                             1000.0f);      // Far plane

        v = glm::mat4(1.0f);

        glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -40.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        v = glm::lookAt(cameraEye,
                        cameraTarget,
                        upVector);

        mvp = p * v * m;

//        glUseProgram(program);
//        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));


        // uniform vec3 colorOverrideRGB;
        GLint colorOverrideRGB_UL = glGetUniformLocation(shaderProgram, "colorOverrideRGB");
        glUniform3f(colorOverrideRGB_UL, 1.0f, 1.0f, 1.0f);

        // GL_POINT, GL_LINE, and GL_FILL
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLES, 0, 3);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
//    exit(EXIT_SUCCESS);
    return 0;
}
