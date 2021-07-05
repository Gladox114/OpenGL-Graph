#include <iostream>
#include "../Windowing.h"
//#include "../OpenGLStuff.h"
#include "../glad/glad.h"
#include <GLFW/glfw3.h>


extern "C" {
    int init() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            std::cout << "Failed to initialize GLAD" << std::endl;
            //exit(-1);
            return -1;
        }
        return 0;
    }

    void mainFunc(Windowing::WindowData* window) {
        
        glClearColor(0.7f,0.7f,0.7f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        std::cout << "I am " << window->m_ID << std::endl;
    }
}