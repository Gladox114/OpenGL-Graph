#include <iostream>
#include "../Windowing.h"
//#include "../imgui/imgui.h"
//#include "../imgui/backends/imgui_impl_glfw.h"
//#include "../OpenGLStuff.h"
#include "../glad/glad.h"
#include <GLFW/glfw3.h>

int globalVarTest = 2;
extern "C" {

    int init(Windowing::WindowData* window) { 
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            std::cout << "Failed to initialize GLAD" << std::endl;
            //exit(-1);
            return -1;
        }
        
        /*ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();*/
        //globalVarTest = 10;
        return 0;
    }

    void mainFunc(Windowing::WindowData* window) {
        /*ImGui::NewFrame();
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        ImGui::End();
        ImGui::Render(); */
        std::cout << globalVarTest << std::endl;
        glClearColor(0.7f,0.7f,0.7f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        std::cout << "I am " << window->m_ID << std::endl;
    }
}
