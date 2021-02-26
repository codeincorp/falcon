#include <string>
#include <typeindex>
#include <vector>

#pragma once

namespace codein {

struct FieldMetaInfo {
    std::string fieldName;
    std::type_index typeIndex;
};

using Metadata = std::vector<FieldMetaInfo>;

}