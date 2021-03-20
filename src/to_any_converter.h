#include <any>
#include <cassert>
#include <functional>
#include <string>
#include <typeindex>

#pragma once

namespace codein {

extern const std::any nullany;

/**
 * @brief Convert a string into ti type which is mapped to a certain value type
 * such as int/float/string.
 *
 * @param ti Type index
 * @param s String value
 * @return Value of type std::any. The type of return value cannot be known in advance.
 *   So, the converted value is contained in any type though each converter
 *   converts a string into a specific type according to ti. 
 */
std::any convertTo(const std::type_index& ti, const std::string& s) noexcept;

std::type_index convertToTypeid(const std::string& typeName);

/// Type index for void type.
extern const std::type_index tiVoid;
/// Type index for bool type.
extern const std::type_index tiBool;
/// Type index for int type.
extern const std::type_index tiInt;
/// Type index for unsigned type.
extern const std::type_index tiUint;
/// Type index for float type.
extern const std::type_index tiFloat;
/// Type index for double type.
extern const std::type_index tiDouble;
/// Type index for string type.
extern const std::type_index tiString;

} // namespace codein
