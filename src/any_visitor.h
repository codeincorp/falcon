/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <any>
#include <functional>
#include <iomanip>
#include <iostream>
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

std::any convertAny(const std::any& lhs, const std::string& typeName);

/**
 * @brief exception for invalid operation such as discrepant type
 * between operands, or unsupported types of operand for operator. 
 */
class UnsupportedOperation {};

}
