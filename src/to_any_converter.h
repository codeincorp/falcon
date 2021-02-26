#include <any>
#include <cassert>
#include <functional>
#include <string>
#include <typeindex>

#pragma once

namespace codein {

template<class T>
constexpr std::pair<const std::type_index, std::function<std::any(const std::string&)>>
    to_any_converter(const std::function<T(const std::string&)> f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](const std::string& s) -> std::any
        {
            return g(s);
        }
    };
}
 
using any_converter_map = std::unordered_map<
    std::type_index,
    std::function<std::any(const std::string&)>
>;

inline std::any convert_to(const any_converter_map& any_converters, const std::type_index& ti, const std::string& s)
{
    if (const auto it = any_converters.find(ti); it != any_converters.cend()) {
        return it->second(s);
    }

    assert(!"unsupported type");

    return std::any();
}
 
extern any_converter_map any_converters;

extern const std::type_index int_ti;
extern const std::type_index float_ti;
extern const std::type_index string_ti;
extern const std::type_index double_ti;

} // namespace codein
