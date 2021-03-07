#include <string>
#include <typeindex>
#include "to_any_converter.h"

namespace codein {

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

const std::type_index tiVoid = std::type_index(typeid(void));
const std::type_index tiBool = std::type_index(typeid(bool));
const std::type_index tiInt = std::type_index(typeid(int));
const std::type_index tiUint = std::type_index(typeid(unsigned));
const std::type_index tiFloat = std::type_index(typeid(float));
const std::type_index tiString = std::type_index(typeid(std::string));
const std::type_index tiDouble = std::type_index(typeid(double));

} // namespace codein
