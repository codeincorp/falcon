#include <initializer_list>
#include <string>
#include <typeindex>
#include <vector>
#include <unordered_map>

#include "to_any_converter.h"

#pragma once

namespace codein {

class UnknownName {};

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

    FieldMetaInfo(const char* fieldName, std::type_index typeIndex)
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
class Metadata {
public:
    Metadata() = default;
    Metadata(const Metadata&) = default;
    Metadata(Metadata&&) = default;

    Metadata(std::initializer_list<FieldMetaInfo> il)
        : fields_(il)
        , colToIdx_()
    {}

    explicit Metadata(std::size_t sz)
        : fields_(sz)
        , colToIdx_()
    {}

    Metadata& operator=(const Metadata&) = default;
    Metadata& operator=(Metadata&&) = default;

    std::pair<bool, std::size_t> find(const std::string& name) const {
         ensureColToIdxSetup();

        auto i = colToIdx_.find(name);
        return i == colToIdx_.cend()
            ? std::pair{false, std::size_t{}}
            : std::pair{true, i->second};
    }

    const FieldMetaInfo& operator[](std::size_t i) const {
        return fields_[i];
    }

    FieldMetaInfo& operator[](std::size_t i) {
        return fields_[i];
    }

    // Writing back is not allowed and always value is returned.
    std::size_t operator[](const std::string& name) const {
        if (auto [found, i] = find(name); found) {
            return i;
        }
        else {
            throw UnknownName();
        }
    }

    std::size_t size() const {
        return fields_.size();
    }

    void reserve(std::size_t sz) {
        fields_.reserve(sz);
    }

    template <typename... ArgTs>
    Metadata& emplace_back(ArgTs&&... args)
    {
        fields_.emplace_back(std::forward<ArgTs>(args)...);
        return *this;
    }

private:
    void ensureColToIdxSetup() const {
        if (fields_.size() == colToIdx_.size()) {
            return;
        }

        for (size_t i = 0; i < fields_.size(); ++i) {
            const auto& fieldName = fields_[i].fieldName;
            colToIdx_[fieldName] = i;
        }
    }

    std::vector<FieldMetaInfo> fields_;
    mutable std::unordered_map<std::string, std::size_t> colToIdx_;
};

inline bool operator==(const Metadata& lhs, const Metadata& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.size(); ++i) {
        if (lhs[i].fieldName != rhs[i].fieldName || lhs[i].typeIndex != rhs[i].typeIndex) {
            return false;
        }
    }

    return true;
}

}
