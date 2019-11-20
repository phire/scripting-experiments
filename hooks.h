#pragma once

#include <vector>
#include <tuple>
#include <stdio.h>
#include <cstdint>
#include <functional>

class HookBase {
public:
     virtual void Register(void *hook, void *data) {};

     std::string getName() { return name; }

protected:
    std::string name;
    std::string type;

    static void registerHook(HookBase *hook);
};


template <typename... Args>
class Hook : public HookBase {
public:
    Hook(std::string name) {
        this->name = name;
        registerHook(this);
    }

    void emit(Args... args) {
        for (auto &hook : hooks) {
            auto fn = std::get<0>(hook);
            auto data = std::get<1>(hook);
            printf("executing hook %llx (%llx)\n", std::get<0>(hook), std::get<1>(hook));
            fn(data, args...);
        }
    }

    void Register(void(*hook)(void *, Args...), void *data) {
        hooks.push_back(std::make_tuple(hook, data));
    }

    virtual void Register(void *hook, void *data) {
        Register(reinterpret_cast<void(*)(void *, Args...)>(hook), data);
    }

private:

    std::vector<std::tuple<void(*)(void *, Args...), void *>> hooks;
};
