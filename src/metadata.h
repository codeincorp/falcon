#include <string>
#include <typeindex>
#include <vector>

#include "to_any_converter.h"

#pragma once

namespace codein {

/**
 * @brief Metadata information for a field such as field name and field type
 */
struct FieldMetaInfo {
    std::string fieldName;
    std::type_index typeIndex;

    FieldMetaInfo(const std::string& fieldName, std::type_index typeIndex)
        : fieldName(fieldName)
        , typeIndex(typeIndex)
    {}

    FieldMetaInfo()
        : FieldMetaInfo("", tiVoid)
    {}
};

/**
 * @brief Metadata information about data
 */
using Metadata = std::vector<FieldMetaInfo>;

}
