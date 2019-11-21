#include "hooks.h"

std::vector<HookBase *> *all_hooks;

std::string fn_list;

void HookBase::registerHook(HookBase *hook) {

    // Since this function is called from statically constructed objects,
    // we can't rely on static initialization of all_hooks.
    if (!all_hooks)
        all_hooks = new std::vector<HookBase *>();

    printf("Registered hook %s\n", hook->getName().c_str());
    all_hooks->push_back(hook);
}


extern "C" {

export const char * list_hooks() {
    if (!all_hooks->empty() && fn_list.empty()) {
        for (auto &hook : *all_hooks) {
            fn_list += hook->getName() + "\n";
        }
    }

    return fn_list.c_str();
}

export void register_hook(char * hook_name, void *callback, void *data) {
    std::string name(hook_name);
    for (auto &hook : *all_hooks) {
        if (hook->getName() == name) {
            hook->Register(callback, data);
            return;
        }
    }
    printf("hook not found for '%s'\n", name.c_str());
}

}