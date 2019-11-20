#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <assert.h>

void init_python() {
    printf("initializing python...\n");
    void *python = dlopen("./libpython.so", RTLD_LAZY);
    if (!python) {
        fprintf(stderr, "%s\n", dlerror());
        exit(-1);
    }
    
    void (*init)();
    init = (void (*)()) dlsym(python, "init");
    assert(init);

    init();
    printf("done\n");
}

int main() {

    init_python();

    printf("Waiting:\n");

    int c;
    while ((c = getchar())) {
        switch (c) {
        case 'q':
            return 0;
        }
    }
}