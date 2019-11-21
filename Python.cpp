#include <pybind11/embed.h> // everything needed for embedding
namespace py = pybind11;

extern "C" {

extern const char* list_hooks();
extern void register_hook(char *name, void *callback, void *data);

void callback(void *data, int a, int b) {
    printf("Callback %i %i\n", a, b);
}

void init() {
    py::scoped_interpreter guard{};

    py::print("Hello, World!");
    py::print(list_hooks());

    register_hook("AwesomeHook", (void *)callback, nullptr);
}

}