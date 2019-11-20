#include <pybind11/embed.h> // everything needed for embedding
namespace py = pybind11;

extern "C" {

extern const char* list_hooks();

void init() {
    py::scoped_interpreter guard{};

    py::print("Hello, World!");
    py::print(list_hooks());
}

}