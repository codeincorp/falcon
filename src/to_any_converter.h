#include <any>
#include <cassert>
#include <functional>
#include <string>
#include <typeindex>

#pragma once

namespace codein {

/**
 * @brief Make a type_index&converter to any pair to register the converter for a specific type.
 * 
 * @tparam T The type of value which the string is converted into
 * @param f The converter function which convert string into a value of T
 * @return a pair of type_index & converter
 */
template<class T>
constexpr std::pair<const std::type_index, std::function<std::any(const std::string&)>>
    toAnyConverter(const std::function<T(const std::string&)> f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](const std::string& s) -> std::any
        {
            return g(s);
        }
    };
}

/**
 * @brief Map between type index and converter function.
 */
using anyConverterMap = std::unordered_map<
    std::type_index,
    std::function<std::any(const std::string&)>
>;

/**
 * @brief Convert a string into ti type which is mapped to a certain type.
 *
 * @param anyConverters Table for converter functions.
 * @param ti Type index
 * @param s String value
 * @return Value of type std::any. The type of return value cannot be known in advance.
 *   So, the converted value is contained in any type though each converter
 *   converts a string into a specific type according to ti. 
 */
inline std::any convertTo(const anyConverterMap& anyConverters, const std::type_index& ti, const std::string& s)
{
    if (const auto it = anyConverters.find(ti); it != anyConverters.cend()) {
        return it->second(s);
    }

    assert(!"unsupported type");

    return std::any();
}
 
extern anyConverterMap anyConverters;

extern const std::type_index tiVoid;
extern const std::type_index tiInt;
extern const std::type_index tiUint;
extern const std::type_index tiFloat;
extern const std::type_index tiDouble;
extern const std::type_index tiString;

} // namespace codein
