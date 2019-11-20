#include <pybind11/embed.h> // everything needed for embedding
namespace py = pybind11;

extern "C" {

void init() {
    py::scoped_interpreter guard{};

    py::print("Hello, World!");
}

}