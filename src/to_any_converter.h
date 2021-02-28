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
template <typename T>
constexpr std::pair<
    const std::type_index,
    std::function<std::any(const std::string&)>
>
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
 * @brief Type alias for map between type index and string-to-value converter function.
 */
using AnyConverterMap = std::unordered_map<
    std::type_index,
    std::function<std::any(const std::string&)>
>;

/**
 * @brief Convert a string into ti type which is mapped to a certain value type
 * such as int/float/string.
 *
 * @param anyConverters Table for converter functions.
 * @param ti Type index
 * @param s String value
 * @return Value of type std::any. The type of return value cannot be known in advance.
 *   So, the converted value is contained in any type though each converter
 *   converts a string into a specific type according to ti. 
 */
inline std::any convertTo(
    const AnyConverterMap& anyConverters,
    const std::type_index& ti,
    const std::string& s
) noexcept
{
    try {
        if (const auto it = anyConverters.find(ti); it != anyConverters.cend()) {
            return it->second(s);
        }
    }
    catch (...) {
        // Absorb any exceptions and return null any value instead.
    }

    return std::any();
}

/**
 * @brief The hash table between type and string-to-value converter function.
 * Converter function's return type is std::any which can contain any value inside it.
 * FieldMetadataInfo provides type info with typeIndex and the type info can be used
 * to convert a string into a value of that type.
 */
extern AnyConverterMap anyConverters;

/// Type index for void type.
extern const std::type_index tiVoid;
/// Type index for int type.
extern const std::type_index tiInt;
/// Type index for uint type.
extern const std::type_index tiUint;
/// Type index for float type.
extern const std::type_index tiFloat;
/// Type index for double type.
extern const std::type_index tiDouble;
/// Type index for string type.
extern const std::type_index tiString;

} // namespace codein
