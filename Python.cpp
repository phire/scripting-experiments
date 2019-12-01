// #include <pybind11/embed.h> // everything needed for embedding
// namespace py = pybind11;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <dlfcn.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern "C" {

extern const char* list_hooks();
extern const char* get_schema();

extern void register_hook(char *name, void *callback, void *data);
}

void callback(void *data, int a, int b) {
    printf("Callback %i %i\n", a, b);
}

int add(int i, int j) {
    return i + j;
}

PYBIND11_EMBEDDED_MODULE(cpp_module, m) {
    std::string schema(get_schema());
    json j = json::parse(schema);

    for (const auto &[hook,args] : j["hooks"].items()) {
        void (*func)(void (*)(void*));
        func = (void (*)(void (*)(void*))) dlsym(RTLD_DEFAULT, ("attach_hook_" + hook).c_str());
        printf("%x\n", func);
        m.def(("attach_hook_" + hook).c_str(), func, ("attach to " + hook + " hook").c_str());
        printf("%s %s\n", hook.c_str(), args.dump().c_str());
    }
}

static PyMethodDef EmbMethods[] = {
    {"numargs", emb_numargs, METH_VARARGS,
     "Return the number of arguments received by the process."},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef EmbModule = {
    PyModuleDef_HEAD_INIT, "emb", NULL, -1, EmbMethods,
    NULL, NULL, NULL, NULL
};

static PyObject*
PyInit_emb(void)
{
    return PyModule_Create(&EmbModule);
}

extern "C" {

void init() {

    PyImport_AppendInittab("emb", &PyInit_emb);

    //py::scoped_interpreter guard{};

    // py::print("Hello, World!");
    // py::print(list_hooks());

    // auto py_module = py::module::import("cpp_module");

    // auto locals = py::dict("fmt"_a="{} + {} = {}", **py_module.attr("__dict__"));
    // assert(locals["a"].cast<int>() == 1);
    // assert(locals["b"].cast<int>() == 2);

    // py::exec(R"(
    //     import cpp_module

    //     print(dir(cpp_module.attach_hook_AwesomeHook))
    // )", py::globals());

}

}