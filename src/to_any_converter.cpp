#include <string>
#include <typeindex>
#include <unordered_map>
#include "to_any_converter.h"

namespace codein {

const std::any nullany;

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

std::any convertTo(const std::type_index& ti, const std::string& s) noexcept
{
    /**
     * @brief The hash table between type and string-to-value converter function.
     * Converter function's return type is std::any which can contain any value inside it.
     * FieldMetadataInfo provides type info with typeIndex and the type info can be used
     * to convert a string into a value of that type.
     */
    static const AnyConverterMap anyConverters {
        toAnyConverter<int>([](const std::string& s) {
            return stoi(s);
        }),
        toAnyConverter<unsigned>([](const std::string& s) {
            return static_cast<unsigned>(stoul(s));
        }),
        toAnyConverter<float>([](const std::string& s) {
            return stof(s);
        }),
        toAnyConverter<double>([](const std::string& s) {
            return stod(s);
        }),
        toAnyConverter<std::string>([](const std::string& s) {
            return s;
        }),
    };

    try {
        if (const auto it = anyConverters.find(ti); it != anyConverters.cend()) {
            return it->second(s);
        }
    }
    catch (...) {
        // Absorb any exceptions and return null any value instead.
    }

    return nullany;
}

const std::unordered_map<std::string, std::type_index> typeMap {
    {"int", std::type_index(typeid(int))},
    {"bool", std::type_index(typeid(bool))},
    {"void", std::type_index(typeid(void))},
    {"uint", std::type_index(typeid(unsigned))},
    {"float", std::type_index(typeid(float))},
    {"string", std::type_index(typeid(std::string))},
    {"double", std::type_index(typeid(double))}
};

std::type_index convertToTypeid(const std::string& typeName)
{
    if(auto it = typeMap.find(typeName); it != typeMap.end()) {
        return it->second;
    }
    else {
        return tiVoid;
    }
}

const std::type_index tiVoid = std::type_index(typeid(void));
const std::type_index tiBool = std::type_index(typeid(bool));
const std::type_index tiInt = std::type_index(typeid(int));
const std::type_index tiUint = std::type_index(typeid(unsigned));
const std::type_index tiFloat = std::type_index(typeid(float));
const std::type_index tiString = std::type_index(typeid(std::string));
const std::type_index tiDouble = std::type_index(typeid(double));

} // namespace codein
