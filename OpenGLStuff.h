#include "Windowing.h"
#include "configReadTest.h"

#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <vector>

namespace OGLS {
    void window_focus_callback(GLFWwindow* window, int focused);

    int initGLFW() {
         // glfw: initialize and configure
        // ------------------------------
        if (!glfwInit()) {
            std::cout << "Couldn't initialise glfw" << std::endl;
            return(1); // EXIT_FAILURE
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif
        return 0;
    }

    int checkWindow(GLFWwindow* window) {
        if (window == NULL){
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            //exit(-1);
            return -1;
        }
        glfwMakeContextCurrent(window);
        return 0;
    }

    int initGlad() {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            std::cout << "Failed to initialize GLAD" << std::endl;
            //exit(-1);
            return -1;
        }
        return 0;
    }


    void spawnWindows(std::vector<Windowing::WindowData*> Windows,int amountOfWindows, cppsecrets::ConfigReader* p){
        for (int i = 0; i<amountOfWindows; i++) {
            // create variables
            int width;
            int height;
            // create a string with the number like width1 and width2 to find the right config variable
            std::string widthS = "width";
            std::string heightS = "height";
            widthS.append(std::to_string(i+1));
            heightS.append(std::to_string(i+1));
            // read the config file
            p->getValue(widthS,width);
            p->getValue(heightS,height);
            // create that window
            Windows.push_back(new Windowing::WindowData(width,height,"test",NULL,NULL));
            // check if the window works and make it current
            if (OGLS::checkWindow(Windows[i]->m_Window)) exit(1);
        }
    }
}
