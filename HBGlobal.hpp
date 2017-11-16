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
std::unique_ptr<T> make_unique(Ts&&... params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}
HONEYBEE_END_NAMESPACE
