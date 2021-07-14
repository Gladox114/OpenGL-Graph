#include <iostream>
#include "shader_s.h"
#include "../Windowing.h"
//#include "../OpenGLStuff.h"
#include "../glad/glad.h"
#include <GLFW/glfw3.h>

// define objects
//                      position            texcoord
float quad[] =  {       -1.0f,-1.0f,0.0f,     0.0f,0.0f,
                        1.0f,-0.5f,0.0f,     1.0f,0.0f,
                        1.0f,1.0f,0.0f,    1.0f,1.0f,
                        -1.0f,1.0f,0.0f,    0.0f,1.0f
};
unsigned int indices[] = {
    0,1,3,
    1,2,3
};

// buffer objects
unsigned int VAO,VBO,EBO;

Shader* testShader;

extern "C" {

    int init(Windowing::WindowData* window) { 
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            std::cout << "Failed to initialize GLAD" << std::endl;
            //exit(-1);
            return -1;
        }

        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1,&EBO);

        // any glVertexAttribPointer or ..EnableVertex.. will be stored inside the VAO
        glBindVertexArray(VAO);
        
        // copy our vertices array in a buffer and bind it to VBO
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,sizeof(quad),quad,GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
        
        // Linking Vertex Attributes
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        // Shader
        testShader = new Shader("functions/shader/test.vs","functions/shader/test.fs");
        return 0;
    }

    void mainFunc(Windowing::WindowData* window) {
        
        glClearColor(0.7f,0.7f,0.7f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        testShader->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

        std::cout << "I am " << window->m_ID << " " << window->m_Window << std::endl;

        glfwSwapBuffers(window->m_Window);
        glfwPollEvents();
    }
}

/*
// this will only be used when the script is called locally.
int main() {
    // init
    if (OGLS::initGLFW()) exit(1);
    // create window object
    Windowing::WindowData WindowObj = Windowing::WindowData(900,500,"test",NULL,NULL);
    if (OGLS::checkWindow(WindowObj.m_Window)) exit(1);
    // init GLAD and the rest
    init(&WindowObj);

      

    while(!glfwWindowShouldClose(WindowObj.m_Window)) {
        mainFunc(&WindowObj);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------

        glfwSwapBuffers(WindowObj.m_Window);
        glfwPollEvents();
    }

    return 0;
}*/