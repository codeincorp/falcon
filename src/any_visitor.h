#include <any>
#include <iomanip>
#include <iostream>
#include <functional>
#include <optional>
#include <typeindex>
#include <unordered_map>
#include <utility>

#pragma once

namespace codein {

template <typename T, typename F>
constexpr std::pair<
    const std::type_index,
    std::function<std::ostream& (std::ostream&, const std::any&)>
>
toAnyVisitor(F const &f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](std::ostream&os, std::any const &a) -> std::ostream&
        {
            static_assert(!std::is_void_v<T>);
            g(os, std::any_cast<T const&>(a));
            return os;
        }
    };
}

using AnyVisitorMap = std::unordered_map<
    std::type_index,
    std::function<std::ostream&(std::ostream&, const std::any&)>
>;

template<class T, class F>
inline void registerAnyVisitor(AnyVisitorMap& anyVisitors, F const& f)
{
    std::cout << "Register visitor for type "
              << std::quoted(typeid(T).name()) << '\n';
    anyVisitors.insert(toAnyVisitor<T>(f));
}

extern AnyVisitorMap anyVisitors;

/*
std::ostream& operator<<(std::ostream& os, const std::any& a);

std::ostream& operator<<(std::ostream& os, const std::vector<std::any>& va);
*/

using AnyBinaryComparer = std::function<bool (const std::any&, const std::any&)>;

template <typename T, typename F>
constexpr std::pair<const std::type_index, AnyBinaryComparer>
toAnyEqVisitor(F const &f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](const std::any& lhs, const std::any& rhs) -> bool
        {
            static_assert(!std::is_void_v<T>);
            return g(std::any_cast<const T&>(lhs), std::any_cast<const T&>(rhs));
        }
    };
}

using AnyBinaryComparerVisitorMap = std::unordered_map<
    std::type_index, AnyBinaryComparer>;

extern AnyBinaryComparerVisitorMap anyEqVisitors;

bool operator==(const std::any& lhs, const std::any& rhs);

}
