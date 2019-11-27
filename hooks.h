#pragma once

#include <vector>
#include <tuple>
#include <stdio.h>
#include <cstdint>
#include <functional>

#include "reflect.h"

class HookBase {
public:
     virtual void Register(void *callback, void *data) = 0;

     std::string getName() { return name; }
     std::string getType() { return type; }

protected:
    std::string name;
    std::string type;

    //static void registerHook(HookBase *hook, std::string name);
};

#define export __attribute__ ((visibility ("default")))

template <typename... Args>
class Hook : public HookBase {
public:
    using Type = void(*)(void *, Args...);

    Hook(std::string Name) {
        this->name = Name;
        this->type = reflect_type<Args...>();
    }

    void emit(Args... args) {
        for (auto &hook : hooks) {
            auto fn = std::get<0>(hook);
            auto data = std::get<1>(hook);
            printf("executing hook %llx (%llx)\n", std::get<0>(hook), std::get<1>(hook));
            fn(data, args...);
        }
    }

    void Register(void(*callback)(void *, Args...), void *data) {
        printf("registered %x %x", callback, data);
        hooks.push_back(std::make_tuple(callback, data));
    }

    virtual void Register(void *callback, void *data) {
        Register(reinterpret_cast<void(*)(void *, Args...)>(callback), data);
    }

private:

    std::vector<std::tuple<void(*)(void *, Args...), void *>> hooks;
};

struct hook_data {
    HookBase* hook;
    std::string name;
};

void registerHook(HookBase *hook);

// This is a helper class just handle static initialization
class _Exporter {
public:
    _Exporter(HookBase *hook) {
        registerHook(hook);
    }
};

// This registers the hook
// Might also export a direct 'attach hook' c function
#define EXPORT_HOOK(hook) extern "C" { \
    export void attach_hook_##hook(decltype(hook)::Type callback, void *data) { \
        hook.Register(callback, data); \
    } \
} \
static _Exporter _##hook_exporter(&hook);

//static __attribute__((__used__)) __attribute__((section("hookinfo"))) hook_data hook_info_##hook { &hook, #hook };
