#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <dlfcn.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern "C" {
    const char* list_hooks();
    const char* get_schema();
}

// PYBIND11_EMBEDDED_MODULE(cpp_module, m) {
//     std::string schema(get_schema());
//     json j = json::parse(schema);

//     for (const auto &[hook,args] : j["hooks"].items()) {
//         void (*func)(void (*)(void*));
//         func = (void (*)(void (*)(void*))) dlsym(RTLD_DEFAULT, ("attach_hook_" + hook).c_str());
//         printf("%x\n", func);
//         m.def(("attach_hook_" + hook).c_str(), func, ("attach to " + hook + " hook").c_str());
//         printf("%s %s\n", hook.c_str(), args.dump().c_str());
//     }
// }


typedef struct {
    PyObject_HEAD
} events_object;

static PyTypeObject eventsType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "dolphin.events",
    .tp_basicsize = sizeof(events_object),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "doc string",
    .tp_new = PyType_GenericNew,
};

static PyMethodDef EmbMethods[] = {
    //{"numargs", emb_numargs, METH_VARARGS,
    // "Return the number of arguments received by the process."},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef DolphinModule = {
    PyModuleDef_HEAD_INIT, "dolphin", NULL, -1, EmbMethods,
    NULL, NULL, NULL, NULL
};

static PyObject*
PyInit_dolphin(void)
{
    PyObject *m;
    if (PyType_Ready(&eventsType) < 0)
        return NULL;

    m = PyModule_Create(&DolphinModule);
    if (!m)
        return NULL;

    Py_INCREF(&eventsType);
    if(PyModule_AddObject(m, "events", (PyObject *) &eventsType) < 0) {
        Py_DECREF(&eventsType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

extern "C" {

void init() {
    printf("%s\n", get_schema());

    PyImport_AppendInittab("dolphin", &PyInit_dolphin);

    Py_Initialize();
    PyRun_SimpleString("import dolphin\n"
                       "print(dir(dolphin))\n"
                       "print(dir(dolphin.events))\n");
}

}