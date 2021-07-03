
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
    std::vector<Windowing::WindowData*> Window;
    OGLS::spawnWindows(Window,amountOfWindows,p);

    // init GLAD
    if (OGLS::initGlad()) exit(1);


    /*
    (repeats every tick)
        Window2 (focused)
    (repeats every <inser time>)
        window1
        window3
    */



    while (true) {}
    
    printf("test\n");
}

void OGLS::window_focus_callback(GLFWwindow* window, int focused) {

}