#include "hooks.h"

std::string fn_list;

std::vector<HookBase *>& all_hooks() {
    static std::vector<HookBase *> hooks;
    return hooks;
}

void registerHook(HookBase *hook) {
    printf("Registered hook %s\n", hook->getName().c_str());
    all_hooks().push_back(hook);
}

extern "C" {

export const char * list_hooks() {
    if (!all_hooks().empty() && fn_list.empty()) {
        for (auto &hook : all_hooks()) {
            fn_list += hook->getName() + "\n";
        }
    }

    return fn_list.c_str();
}

export void register_hook(char * hook_name, void *callback, void *data) {
    std::string name(hook_name);
    for (auto &hook : all_hooks()) {
        if (hook->getName() == name) {
            hook->Register(callback, data);
            return;
        }
    }
    printf("hook not found for '%s'\n", name.c_str());
}

}