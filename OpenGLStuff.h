#ifndef OPENGLSTUFF_H
#define OPENGLSTUFF_H
#include "Windowing.h"
#include "configReadTest.h"
#include "libraryLoader.h"

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
    
    std::string getConfVal(std::string string,int x) {
        string.append(std::to_string(x+1));
        return string;
    }

    void defaultFunc(Windowing::WindowData* window) {
        // rendering commands
        // ------------------
        //glClearColor(0.2f,0.2f,0.2f,1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        std::cout << "I am " << window->m_ID << " " << window->m_Window << std::endl;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------        
        glfwSwapBuffers(window->m_Window);
        glfwPollEvents();
    }
    

    void spawnWindows(std::vector<Windowing::WindowData*> &Windows,int amountOfWindows, cppsecrets::ConfigReader* p){
        for (int i = 0; i<amountOfWindows; i++) {
            // create variables
            int width;
            int height;

            // read the config file
            p->getValue(getConfVal("width",i),width);
            p->getValue(getConfVal("height",i),height);

            // create that window
            Windows.push_back(new Windowing::WindowData(width,height,"test",NULL,NULL));
            // change some configs
            Windows[i]->m_ID = i;

            // check if the window works and make it current
            if (OGLS::checkWindow(Windows[i]->m_Window)) exit(1);
            glfwSetWindowFocusCallback(Windows[i]->m_Window, OGLS::window_focus_callback);
        }
    }

    void attachFunctions(std::vector<Windowing::WindowData*> &Windows,int amountOfWindows, cppsecrets::ConfigReader* p){
        for (int i = 0; i<amountOfWindows; i++) {
            std::string libName;
            std::string funcName;
            p->getValue(getConfVal("libName",i),libName);
            p->getValue(getConfVal("funcName",i),funcName);
            if (libName.empty() || funcName.empty())
                Windows[i]->mainFunction = defaultFunc; // to at least have a function apply the default function
            else {
                // load the library.so
                // append to the location the libName and then the file ending .so to get the full location. Also convert them to char*
                void* library = libLoader::loadLib(std::string("./functions/").append(libName.append(".so")).c_str());
                // grab the init function and execute it
                libLoader::init_func init = (libLoader::init_func) libLoader::loadFunc(library, "init");
                printf("test: %i \n",init(Windows[i]));
                // grab the main function and save it to the window
                Windows[i]->mainFunction = libLoader::loadFunc(library, funcName.c_str());
            }
        }
    }

}
#endif