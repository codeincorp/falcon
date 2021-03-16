#include <string>
#include <typeindex>
#include <unordered_map>
#include "to_any_converter.h"

namespace codein {

const std::any nullany;

AnyConverterMap anyConverters {
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
