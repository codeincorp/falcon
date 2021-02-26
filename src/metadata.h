#include <string>
#include <typeindex>
#include <vector>

#pragma once

namespace codein {

struct FieldMetaInfo {
    std::string fieldName;
    std::type_index typeIndex;

    FieldMetaInfo(const std::string& fieldName, std::type_index typeIndex)
        : fieldName(fieldName)
        , typeIndex(typeIndex)
    {}

    FieldMetaInfo()
        : fieldName("")
        , typeIndex(typeid(int))
    {}
};

using Metadata = std::vector<FieldMetaInfo>;

}