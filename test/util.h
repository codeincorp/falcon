#include <any>
#include <vector>
#include <memory>

#include "iterator.h"

#pragma once

void verifyIteratorOutput(const std::vector<std::vector<std::any>>& expectedFields, const std::unique_ptr<codein::Iterator>& iterator);