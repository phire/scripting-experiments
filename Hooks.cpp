#include "hooks.h"
#include "structs.h"

#include <map>

std::string fn_list;

std::vector<StructBase *>& all_structs() {
    // We can't rely on static initialization order
    // So this function initializes all_hooks the first time it's called;
    static std::vector<StructBase *> _all_structs;
    return _all_structs;
}

void registerStruct(StructBase *s) {
    all_structs().push_back(s);
}

std::vector<HookBase *>& all_hooks() {
    // We can't rely on static initialization order
    // So this function initializes all_hooks the first time it's called;
    static std::vector<HookBase *> _all_hooks;
    return _all_hooks;
}

void registerHook(HookBase *hook) {
    printf("Registered hook %s\n", hook->getName().c_str());
    all_hooks().push_back(hook);
}
 
std::map<std::string, const StructBase *> types = {
    {"string", nullptr},
    {"int32_t", nullptr}
};

bool initHooks() {
    auto structs = all_structs();

    while (structs.size() != 0) {
        std::string unknown_type_name;
        std::string unknown_type;
        size_t before_size = structs.size();

        auto end = std::remove_if(structs.begin(), structs.end(), [&](auto const *s) {
                for (auto *member : s->members ) {
                    if (types.find(member->Type) == types.end()) {
                        unknown_type = s->Name + "." + member->Name;
                        unknown_type = member->Type; // Try to provide some helpful logs
                        return false;
                    }
                }
                return true;
            });

        
        if (end == structs.end()) {
            // The number of structs didn't decrease 
            printf("Couldn't initialize hooks: Struct Member %s depends on unknown type %s\n", unknown_type_name.c_str(), unknown_type.c_str());
            return false;
        }

        std::for_each(end, structs.end(), [&](auto const *s) {
                types[s->Type] = s;
            });

        structs.erase(end, structs.end());

    }

    for (auto *hook : all_hooks()) {
        for (auto &arg : hook->arg_types) {
            if (types.find(arg) == types.end()) {
                printf("Couldn't initialize hooks: %s arg depends on unknown type %s\n", hook->name.c_str(), arg.c_str());
                return false;
            }
        }
    }
    return true;
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

