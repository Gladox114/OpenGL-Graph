#include <iostream>
#include "../Windowing.h"
//#include "../OpenGLStuff.h"
#include "../glad/glad.h"
#include <GLFW/glfw3.h>


extern "C"
void mainFunc(Windowing::WindowData* window) {
    
    glClearColor(0.7f,0.7f,0.7f,1.0f);
    std::cout << "I am " << window->m_ID << std::endl;
}
