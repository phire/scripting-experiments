#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <assert.h>

#include "hooks.h"
#include "structs.h"

void init_python() {
    printf("initializing python...\n");
    void *python = dlopen("./libpython.so", RTLD_NOW | RTLD_GLOBAL);
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

struct ccc {
    std::string foo;
    std::string bar;
};

static Hook<int, int, ccc> AwesomeHook("AwesomeHook");
EXPORT_HOOK(AwesomeHook);

EXPORT_STRUCT(ccc);
EXPORT_STRUCT_MEMBER(ccc, foo);
EXPORT_STRUCT_MEMBER(ccc, bar);

int main() {

    init_python();

    printf("Waiting:\n");
    printf("%s\n", AwesomeHook.getType().c_str());

    int c;
    while ((c = getchar())) {
        switch (c) {
        case 'q':
            return 0;
        case 'a':
            AwesomeHook.emit(2, 3, {});
            break;
        }
    }
}