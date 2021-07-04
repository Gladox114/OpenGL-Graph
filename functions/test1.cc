#include <iostream>
#include "../Windowing.h"

extern "C"
void mainFunc(Windowing::WindowData* window) {

    std::cout << "I am " << window->m_ID << std::endl;

}
