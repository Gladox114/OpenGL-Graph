
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


int currentWindow = 0;
int tickrate = 20;

int main() {
    system("cd ./functions && ./compileSO.sh");
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

    OGLS::attachFunctions(WindowObjs,amountOfWindows,p);

    /*
    (repeats every tick)
        Window2 (focused)
    (repeats every <insert time>)
        window1
        window3
    *///


    while (!glfwWindowShouldClose(WindowObjs[0]->m_Window)) {
        // render the focused window
        for (int i = 0; i<tickrate; i++) {
            glfwMakeContextCurrent(WindowObjs[currentWindow]->m_Window);
            WindowObjs[currentWindow]->run();
        }
        
        // render the rest of the windows 
        for (int i = 0; i<WindowObjs.size();i++) {
            if (i != currentWindow) {
                glfwMakeContextCurrent(WindowObjs[i]->m_Window);
                WindowObjs[i]->run();
            }
        }
    }
    
    printf("test\n");
}

void OGLS::window_focus_callback(GLFWwindow* window, int focused) {
    // get the class of the current window
    Windowing::WindowData* WindowObj = reinterpret_cast<Windowing::WindowData*>(glfwGetWindowUserPointer(window));
    if (focused)
    {
        std::cout << "a window got focused " << WindowObj->m_ID << std::endl;
        currentWindow = WindowObj->m_ID;
    }
    else
    {
        std::cout << "unfocused window " << WindowObj->m_ID << std::endl;
    }
}