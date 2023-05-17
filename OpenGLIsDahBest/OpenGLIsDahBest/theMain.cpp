//#include <glad/glad.h>
//#define GLFW_INCLUDE_NONE
//#include <GLFW/glfw3.h>
#include "globalStuff.h"

//1026434 Evan Benitez

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
#include <fstream>
using namespace std;





unsigned int numVerticesLoaded = 0;
unsigned int numTrianglesLoaded = 0;

struct sVertexXYZ_RGB
{
    float x, y, z;      // vec2 to vec3 
    float r, g, b;
};



static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

sVertexXYZ_RGB* pVertexArray = NULL;

bool Load_Plyfile(std::string filename,
                    sVertexXYZ_RGB* &pVertexArray,
                    unsigned int &numVerticesLoaded,
                    unsigned int &numTrianglesLoaded);



int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer;

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



    if (Load_Plyfile("dolphin_xyz_rgba.ply", pVertexArray, numVerticesLoaded, numTrianglesLoaded))
    {
        std::cout<<"File loaded OK" <<std::endl;
    }
    else
    {
        std::cout << "Error" << std::endl;
    }
        

    unsigned int numberOfBytes = sizeof(sVertexXYZ_RGB) * numTrianglesLoaded * 3;

    
    glBufferData(GL_ARRAY_BUFFER, 
                 numberOfBytes,    // Each vertex in bytes
                 pVertexArray,                  // Pointer to the start of the array
                 GL_STATIC_DRAW);


    delete[] pVertexArray;

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
                                           //0.0f, 
                                           (float)glfwGetTime(),
                                           glm::vec3(0.0f, 1.0f, 0.0f));
//        mat4x4_mul(mvp, p, m);
        m = matRotateZ * m;


//        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

        p = glm::perspective(0.6f,
                             ratio,
                             0.1f,          // Near plane
                             1000.0f);      // Far plane

        v = glm::mat4(1.0f);

        glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -600.0f);
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

        //glDrawArrays(GL_TRIANGLES, 0, 3);
        unsigned int numberOfVetricesToDraw = numTrianglesLoaded * 3;
        glDrawArrays(GL_TRIANGLES, 0, numberOfVetricesToDraw);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
//    exit(EXIT_SUCCESS);
    return 0;
}

bool Load_Plyfile(  std::string filename,
                    sVertexXYZ_RGB* &pVertexArray,
                    unsigned int &numVerticesLoaded,
                    unsigned int &numTrianglesLoaded )
{
    std::ifstream fileToLoad(filename.c_str());

    if (fileToLoad.is_open() == false)
    {
        return false;
    }


    std::string tempToken;
    while (fileToLoad >> tempToken)
    {
        if (tempToken == "vertex")
        {
            break;
        }
    }

    fileToLoad >> numVerticesLoaded;

    while (fileToLoad >> tempToken)
    {
        if (tempToken == "face")
        {
            break;
        }
    }

    fileToLoad >> numTrianglesLoaded;

    while (fileToLoad >> tempToken)
    {
        if (tempToken == "end_header")
        {
            break;
        }
    }


    struct sTempPlyFileVertex 
    {
        float x;
        float y;
        float z;
        int red;
        int green;
        int blue;
        int alpha;

    };

    std::vector< sTempPlyFileVertex> VectorVertexFromFile;

    for (unsigned int count = 0; count < numVerticesLoaded; count++)
    {
        sTempPlyFileVertex tempVertexLoaded;

        fileToLoad >> tempVertexLoaded.x;
        fileToLoad >> tempVertexLoaded.y;
        fileToLoad >> tempVertexLoaded.z;
        fileToLoad >> tempVertexLoaded.red;
        fileToLoad >> tempVertexLoaded.green;
        fileToLoad >> tempVertexLoaded.blue;
        fileToLoad >> tempVertexLoaded.alpha;

        VectorVertexFromFile.push_back(tempVertexLoaded);

    }

    struct sTempPlyFileTriangle
    {
        unsigned int vertex_0;
        unsigned int vertex_1;
        unsigned int vertex_2;

    };

    std::vector< sTempPlyFileTriangle> VectorTrianglesFromFile;

    for (unsigned int count = 0; count < numTrianglesLoaded; count++)
    {
        sTempPlyFileTriangle tempTrianglesLoaded;

        unsigned int discardThis = 0;
        fileToLoad >> discardThis;

        fileToLoad >> tempTrianglesLoaded.vertex_0;
        fileToLoad >> tempTrianglesLoaded.vertex_1;
        fileToLoad >> tempTrianglesLoaded.vertex_2;

        VectorTrianglesFromFile.push_back(tempTrianglesLoaded);

    }

    unsigned int totalNumberOfVerticesToDraw = numTrianglesLoaded * 3;

    pVertexArray = new sVertexXYZ_RGB[totalNumberOfVerticesToDraw];

    unsigned int finalVertexArrayIndex = 0;

    for (unsigned int triangleIndex = 0; triangleIndex != VectorTrianglesFromFile.size(); triangleIndex++)
    {
        unsigned int vertexIndex_0 = VectorTrianglesFromFile[triangleIndex].vertex_0;

        pVertexArray[finalVertexArrayIndex].x = VectorVertexFromFile[vertexIndex_0].x;
        pVertexArray[finalVertexArrayIndex].y = VectorVertexFromFile[vertexIndex_0].y;
        pVertexArray[finalVertexArrayIndex].z = VectorVertexFromFile[vertexIndex_0].z;

        pVertexArray[finalVertexArrayIndex].r = static_cast<float>(VectorVertexFromFile[vertexIndex_0].red / 255.0f);
        pVertexArray[finalVertexArrayIndex].g = static_cast<float>(VectorVertexFromFile[vertexIndex_0].green / 255.0f);
        pVertexArray[finalVertexArrayIndex].b = static_cast<float>(VectorVertexFromFile[vertexIndex_0].blue / 255.0f);
        finalVertexArrayIndex++;


        unsigned int vertexIndex_1 = VectorTrianglesFromFile[triangleIndex].vertex_1;

        pVertexArray[finalVertexArrayIndex].x = VectorVertexFromFile[vertexIndex_1].x;
        pVertexArray[finalVertexArrayIndex].y = VectorVertexFromFile[vertexIndex_1].y;
        pVertexArray[finalVertexArrayIndex].z = VectorVertexFromFile[vertexIndex_1].z;

        pVertexArray[finalVertexArrayIndex].r = static_cast<float>(VectorVertexFromFile[vertexIndex_1].red / 255.0f);
        pVertexArray[finalVertexArrayIndex].g = static_cast<float>(VectorVertexFromFile[vertexIndex_1].green / 255.0f);
        pVertexArray[finalVertexArrayIndex].b = static_cast<float>(VectorVertexFromFile[vertexIndex_1].blue / 255.0f);
        finalVertexArrayIndex++;

        unsigned int vertexIndex_2 = VectorTrianglesFromFile[triangleIndex].vertex_2;

        pVertexArray[finalVertexArrayIndex].x = VectorVertexFromFile[vertexIndex_2].x;
        pVertexArray[finalVertexArrayIndex].y = VectorVertexFromFile[vertexIndex_2].y;
        pVertexArray[finalVertexArrayIndex].z = VectorVertexFromFile[vertexIndex_2].z;

        pVertexArray[finalVertexArrayIndex].r = static_cast<float>(VectorVertexFromFile[vertexIndex_2].red / 255.0f);
        pVertexArray[finalVertexArrayIndex].g = static_cast<float>(VectorVertexFromFile[vertexIndex_2].green / 255.0f);
        pVertexArray[finalVertexArrayIndex].b = static_cast<float>(VectorVertexFromFile[vertexIndex_2].blue / 255.0f);


        finalVertexArrayIndex++;

    }

    fileToLoad.close();

    return true;

}
