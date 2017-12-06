#pragma once

#include <memory>
#include <cstdlib>
#include <functional>
#include <iostream>

#define HONEYBEE_NAME_SPACE honeybee
#define HONEYBEE_BEGIN_NAMESPACE namespace HONEYBEE_NAME_SPACE {
#define HONEYBEE_END_NAMESPACE }


HONEYBEE_BEGIN_NAMESPACE
template<typename T, typename... Ts>
std::unique_ptr<T> inline make_unique(Ts&&... params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

template<typename T> inline void destroy(T *ptr) {
    delete ptr;
    ptr = nullptr;
}

#define UNUSED(expr) do { (void)(expr); } while (0)

HONEYBEE_END_NAMESPACE
