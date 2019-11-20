#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <assert.h>

#include "hooks.h"

void init_python() {
    printf("initializing python...\n");
    void *python = dlopen("./libpython.so", RTLD_NOW);
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

static Hook<int, int> AwesomeHook("AwesomeHook");

int main() {

    init_python();

    printf("Waiting:\n");

    int c;
    while ((c = getchar())) {
        switch (c) {
        case 'q':
            return 0;
        case 'a':
            AwesomeHook.emit(2, 3);
            break;
        }
    }
}