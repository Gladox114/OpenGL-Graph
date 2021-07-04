#include "Windowing.h"

#include <iostream>
#include <fstream>

#include <dlfcn.h>

namespace libLoader {
    typedef void (*custom_func)(Windowing::WindowData*);

    custom_func loadFunc(const char* location,const char* functionName) {
        void* function = dlopen(location,RTLD_LAZY);
        if(!function)
            printf("Couldn't load function\n");
        
        dlerror();
        custom_func func = (custom_func) dlsym(function,functionName);
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            std::cerr << "Cannot load symbol '" << functionName << "': " << dlsym_error << std::endl;
            dlclose(function);
            return NULL;
        }
        return func;
    }
}
