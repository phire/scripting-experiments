#include "hooks.h"
#include "structs.h"

#include <map>
#include <fmt/format.h>


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
    fmt::print("Registered hook {}\n", hook->getName());
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
                    fmt::print("{} {}\n", member->Name, member->Type);
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
            fmt::print("Couldn't initialize hooks: Struct Member {} depends on unknown type {}\n", unknown_type_name, unknown_type);
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
                fmt::print("Couldn't initialize hooks: {} arg depends on unknown type {}\n", hook->name, arg);
                return false;
            }
        }
    }
    return true;
}

std::string get_type_name(std::string type_id) {
    if (types[type_id] == nullptr)
        return type_id;
    return types[type_id]->Name;
}

std::string trim_last_comma(std::string str) {
    // Assume last was ",\n"
    return str.substr(0, str.size()-2) + '\n';
}

std::string generate_json_schema() {
    // TODO: less janky json formatting.

    std::string str = "{\n\t\"types\": {\n";

    for (auto const &[type_id, info] : types) {
        std::string name(type_id);
        if (info) {
            name = info->Name;
        }
        str += fmt::format("\t\t\"{}\": {{", name);

        if (info) {
            str += "\n";
            str += fmt::format("\t\t\t\"total_size\": {},\n", info->Size);
            str += fmt::format("\t\t\t\"members\": {{\n");
            for (auto const &member : info->members) {
                str += fmt::format("\t\t\t\t\"{}\": {{\n", member->Name);
                str += fmt::format("\t\t\t\t\t\"type\": \"{}\",\n", get_type_name(member->Type));
                // str += fmt::format("\t\t\t\t\t\"size\": {},\n", member->Size);
                str += fmt::format("\t\t\t\t\t\"offset\": {}\n", member->Offset);
                str += fmt::format("\t\t\t\t}},\n", member->Name);
            }
            str = trim_last_comma(str);
            str += fmt::format("\t\t\t}}\n");

            str += "\t\t";
        }

        str += "},\n";
    }
    str = trim_last_comma(str);

    str += "\t},\n";
    str += "\t\"hooks\": {\n";

    for (auto const *hook : all_hooks()) {
        str += fmt::format("\t\t\"{}\": [\n", hook->name);
        for (auto const &arg : hook->arg_types) {
            str += fmt::format("\t\t\t\"{}\",\n", get_type_name(arg));
        }
        str = trim_last_comma(str);
        str += "\t\t],\n";
    }
    str = trim_last_comma(str);

    str += "\t}\n";

    return str + "}\n";
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

export const char *get_schema() {
    static std::string schema = generate_json_schema();
    return schema.c_str();
}

export void register_hook(char * hook_name, void *callback, void *data) {
    std::string name(hook_name);
    for (auto &hook : all_hooks()) {
        if (hook->getName() == name) {
            hook->Register(callback, data);
            return;
        }
    }
    fmt::print("hook not found for '{}'\n", name);
}

}

