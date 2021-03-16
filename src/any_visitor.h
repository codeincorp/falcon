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

bool operator==(const std::any& lhs, const std::any& rhs);
bool operator!=(const std::any& lhs, const std::any& rhs);
bool operator<(const std::any& lhs, const std::any& rhs);
bool operator<=(const std::any& lhs, const std::any& rhs);
bool operator>(const std::any& lhs, const std::any& rhs);
bool operator>=(const std::any& lhs, const std::any& rhs);

std::any operator+(const std::any& lhs, const std::any& rhs);
std::any operator-(const std::any& lhs, const std::any& rhs);
std::any operator*(const std::any& lhs, const std::any& rhs);
std::any operator/(const std::any& lhs, const std::any& rhs);
std::any operator%(const std::any& lhs, const std::any& rhs);

bool notAny(const std::any& lhs);

std::size_t hashAny(const std::any& lhs);

}
