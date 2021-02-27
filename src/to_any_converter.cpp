#include <string>
#include <typeindex>
#include "to_any_converter.h"

namespace codein {

any_converter_map any_converters {
    to_any_converter<int>([](const std::string& s){ return stoi(s); }),
    to_any_converter<unsigned>([](const std::string& s){ return static_cast<unsigned>(stoul(s)); }),
    to_any_converter<float>([](const std::string& s){ return stof(s); }),
    to_any_converter<double>([](const std::string& s){ return stod(s); }),
    to_any_converter<std::string>([](const std::string& s){ return s; }),
};

const std::type_index void_ti = std::type_index(typeid(void));
const std::type_index int_ti = std::type_index(typeid(int));
const std::type_index uint_ti = std::type_index(typeid(uint));
const std::type_index float_ti = std::type_index(typeid(float));
const std::type_index string_ti = std::type_index(typeid(std::string));
const std::type_index double_ti = std::type_index(typeid(double));

} // namespace codein
