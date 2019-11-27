#pragma once

#include <typeinfo>
#include <map>

template<typename T, typename... Args>
std::string reflect_type() {
    if constexpr (sizeof...(Args)) {
        return reflect_type<T>() + " " + reflect_type<Args...>();
    }

    if constexpr (std::is_class<T>::value) {
        // 
        return std::string("class<") + std::to_string(typeid(T).hash_code()) + ", " + typeid(T).name() + ">";
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
            return ("float");
        }
        if constexpr(std::is_same<T, double>::value) {
            return ("double");
        }
    }
    if constexpr(std::is_same<T, std::string>::value) {
        return ("string");
    }
}


// constexpr auto reflect_type() {
//     return reflect_type<T>() + " " + reflect_type<Args...>();
// }

/*
constexpr auto map() {
    return std::map<type_info, char *> {

    }
}*/
/*

template<typename T>
constexpr auto reflect() {
    auto t = typeid(T);

    if (t == typeid(int32_t)) {
        return "int32_t";
    } else if (t == typeid(std::string)) {
        return "string";
    } else if (t == typeid(std::vector)) {
        return "vector";
    } else if (Is)
    return "unknown";
}*/