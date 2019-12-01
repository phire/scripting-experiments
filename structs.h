
#include <string>
#include <utility>
#include "reflect.h"

struct StructInfo {
    std::string type;
};

template<typename T>
struct derrive_c_type {
    using Type = char *;
};

template<typename T, typename U>
constexpr T* AtOffset(U *obj, size_t offset) {
    return reinterpret_cast<T*>(reinterpret_cast<void*>(obj) + offset);
}

struct StructMemberBase {
    std::string Name;
    size_t Offset;
    size_t Size;
    std::string Type;
};

struct StructBase {
    std::string Name;
    std::string Type;
    size_t Size;
    std::vector<StructMemberBase*> members;
};

void registerStruct(StructBase *);

template<typename T>
struct StructExporter : public StructBase {
    using Ty =  T;

    StructExporter(std::string name) {
        Name = name;
        Type = reflect_type<T>();
        Size = sizeof(T);
        registerStruct(this);
    }

    void registerMember(StructMemberBase *member) {
        members.push_back(member);
    }
};

template<typename T, typename U>
struct StructMember : public StructMemberBase {
    using CType = typename derrive_c_type<T>::Type;
    StructMember(StructExporter<U> &exporter, size_t offset, std::string name) {
        Name = name;
        Offset = offset;
        Size = sizeof(T);
        Type = reflect_type<T>();

        exporter.registerMember(this);
    }

    CType get(U *obj) {
        if constexpr(std::is_same<CType, T>::value) {
            return *AtOffset(obj, Offset);
        }
    }
    void set(U *obj, T arg) {
        if constexpr(std::is_same<CType, T>::value) {
            *AtOffset(obj, Offset) = arg;
        }
    }
};

template<typename T>
struct StructMethod : public StructInfo {
};

#define EXPORT_STRUCT(ty) static StructExporter<ty> _##ty##_exporter( #ty )
#define EXPORT_STRUCT_NAME(name, ty) static StructExporter<ty> _##name##_exporter( #name )

#define EXPORT_STRUCT_MEMBER(ty_name, m_ty) \
    static StructMember<decltype(((decltype(_##ty_name##_exporter)::Ty*)(nullptr))->m_ty), decltype(_##ty_name##_exporter)::Ty> \
        _##ty_name##_##m_ty##_exporter( _##ty_name##_exporter, offsetof(decltype(_##ty_name##_exporter)::Ty, m_ty), #m_ty);
    // extern "C" export void* _##ty##_##m_ty##_get(void *_object) { \
    //    auto object = reinterpret_cast<ty*>(_object); \
    //    return reinterpret_cast<void*>( _##ty##_##m_ty##_exporter.get(object)); \
    // }

//decltype((decltype(_##ty_name##_exporter)::Ty)::m_ty)
    
//#define EXPORT_STRUCT_MEMBER(ty, m_ty) \
//    EXPORT_STRUCT_MEMBER_READONLY(ty, m_ty)
    // extern "C" export void _##ty##_##m_ty##_set(void *_object, void *_arg) { \
    //     auto object = reinterpret_cast<ty*>(_object); \
    //     auto arg = reinterpret_cast< decltype(_##ty##_##m_ty##_exporter)::CType>(_arg); \
    //     _##ty##_##m_ty##_exporter.set(object, arg); \
    // }
