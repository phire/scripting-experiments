#include "hooks.h"

std::vector<HookBase *> all_hooks;

std::string fn_list;

void HookBase::registerHook(HookBase *hook) {
    printf("Registered hook %s\n", hook->getName().c_str());
    all_hooks.push_back(hook);
}


extern "C" {

const char * list_hooks() {
    if (!all_hooks.empty() && fn_list.empty()) {
        for (auto &hook : all_hooks) {
            fn_list += hook->getName() + "\n";
        }
    }

    return fn_list.c_str();
}
}