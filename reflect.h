#pragma once

#include <typeinfo>
#include <string>
#include <vector>

template<typename T>
std::string reflect_type() {

    if constexpr(std::is_same<T, std::string>::value) {
        return "string";
    }
    if constexpr (std::is_integral<T>::value) {
        constexpr size_t width = std::numeric_limits<T>::digits;
        if constexpr(std::is_signed<T>::value) {
            return "int" + std::to_string(width + 1) + "_t";
        } else {
            return "uint" + std::to_string(width) + "_t";
        }
    }
    if constexpr(std::is_floating_point<T>::value) {
        if constexpr(std::is_same<T, float>::value) {
            return "float";
        }
        if constexpr(std::is_same<T, double>::value) {
            return "double";
        }
    }
    if constexpr (std::is_class<T>::value) {
        // These class names are internal and not consistent between builds
        // (or potentially even launches)
        // They should never be exposed externally.
        return std::string("class<") + std::to_string(typeid(T).hash_code()) + ", " + typeid(T).name() + ">";
    }
}

template<typename Last>
void reflect_types_inner(std::vector<std::string> &vec) { 
    vec.push_back(reflect_type<Last>());
}

template<typename First, typename Second, typename... Rest>
void reflect_types_inner(std::vector<std::string> &vec) {
    vec.push_back(reflect_type<First>());
    reflect_types_inner<Second, Rest...>(vec);
}

template<typename T, typename... Args>
std::vector<std::string> reflect_types() {
    std::vector<std::string> vec;
    reflect_types_inner<T, Args...>(vec);
    return vec;
}