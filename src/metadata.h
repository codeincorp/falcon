#include <string>
#include <typeindex>
#include <vector>

#include "to_any_converter.h"

#pragma once

namespace codein {

/** Metadata information for a field such as field name and field type */
struct FieldMetaInfo {
    std::string fieldName; /** field name */
    std::type_index typeIndex; /** field type in terms of type index */

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
