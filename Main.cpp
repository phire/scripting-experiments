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

struct TestStructPleaseIgnore {
    std::string foo;
    std::string bar;
};

static Hook<int, int, TestStructPleaseIgnore> AwesomeHook("AwesomeHook");
EXPORT_HOOK(AwesomeHook);

EXPORT_STRUCT(TestStructPleaseIgnore);
EXPORT_STRUCT_MEMBER(TestStructPleaseIgnore, foo);
EXPORT_STRUCT_MEMBER(TestStructPleaseIgnore, bar);

int main() {
    if (!initHooks()) {
        printf("Error initializing hooks\n");
        return -1; 
    }

    fmt::print("{}", generate_json_schema());
    return 0;

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