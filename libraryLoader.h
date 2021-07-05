#include "Windowing.h"

#include <iostream>
#include <fstream>

#include <dlfcn.h>

namespace libLoader {
    typedef void (*custom_func)(Windowing::WindowData*);
    typedef int (*init_func)();

    void* loadLib(const char* location) {
        void* library = dlopen(location,RTLD_LAZY);
        if(!library)
            printf("Couldn't load library\n");
        return library;
    }

    custom_func loadFunc(void* library,const char* functionName) {

        dlerror();
        custom_func func = (custom_func) dlsym(library,functionName);
        const char* dlsym_error = dlerror();
        if (dlsym_error) {
            std::cerr << "Cannot load symbol '" << functionName << "': " << dlsym_error << std::endl;
            dlclose(library);
            return NULL;
        }
        return func;
    }

}
