
#include "OpenGLStuff.h"

#include "configReadTest.h"

#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include "shader_s.h"
#include <array>
#include <ctime>
#include <chrono>
#include <vector>




int main() {
    // -- read config file --
    // ----------------------
    // Create object of the class ConfigReader
    cppsecrets::ConfigReader* p = cppsecrets::ConfigReader::getInstance();
    
    // parse the configuration file
    p->parseFile("config");

    // Define variables to store the value
    int amountOfWindows = 0;

    // Update the variable by the value present in the configuration file.
    p->getValue("amountOfWindows", amountOfWindows);

    // -- init GLFW --
    // ---------------
    // init GLFW
    if (OGLS::initGLFW()) exit(1);
    
    // create windows
    std::vector<Windowing::WindowData*> WindowObjs;
    OGLS::spawnWindows(WindowObjs,amountOfWindows,p);

    // init GLAD
    if (OGLS::initGlad()) exit(1);


    /*
    (repeats every tick)
        Window2 (focused)
    (repeats every <inser time>)
        window1
        window3
    */

    glfwMakeContextCurrent(WindowObjs[0]->m_Window);
    while (!glfwWindowShouldClose(WindowObjs[0]->m_Window)) {
        // rendering commands
        // ------------------
        glClearColor(0.2f,0.2f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(WindowObjs[0]->m_Window);
        glfwPollEvents();
    }

    
    printf("test\n");
}

void OGLS::window_focus_callback(GLFWwindow* window, int focused) {
    // get the class of the current window
    Windowing::WindowData* WindowObj = reinterpret_cast<Windowing::WindowData*>(glfwGetWindowUserPointer(window));
    if (focused)
    {
        std::cout << "a window got focused " << WindowObj->m_ID << std::endl;
    }
    else
    {
        std::cout << "unfocused window " << WindowObj->m_ID << std::endl;
    }
}