
#include "glad/glad.h"
#include <GLFW/glfw3.h>

class GraphBody;
class WindowData;
class PlotData;

/*
|Window
|--GraphBody1
|----PlotData1
|----PlotData2
|--GraphBody2
|----PlotData1
*/
namespace Windowing {
    class WindowData {

        public:
            GLFWwindow* m_Window; // the window ID
            GraphBody* graphBody; // the graphBody ID
            unsigned int* VAO; // vertex array
            unsigned int* VBO; // vertex buffer 
            int m_ID;
            WindowData( int width,
                        int height,
                        const char* title,
                        GLFWmonitor* monitor,
                        GLFWwindow* share, 
                        int m_ID) : m_ID(m_ID)
            {   
                // create window and store the ID in the class
                m_Window = glfwCreateWindow(width, height, title, monitor, share);
                // in GLFW you can have for the current window a pointer. Let's point it's own class to it
                glfwSetWindowUserPointer(m_Window, reinterpret_cast<void*>(this));
            }
            // on destroying the class destroy the window also
            ~WindowData()
            {
                glfwDestroyWindow(m_Window);
            }
    };

}
