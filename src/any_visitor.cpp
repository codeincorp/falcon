#include <any>
#include <cassert>
#include <functional>
#include <string>
#include <typeindex>

#include "any_visitor.h"

namespace codein {

template <typename T>
struct StreamOutputOp {
    void operator()(std::ostream& os, const T& x) const {
        os << x;
    }
};

AnyVisitorMap anyVisitors {
    toAnyVisitor<int>(StreamOutputOp<int>()),
    toAnyVisitor<uint>(StreamOutputOp<uint>()),
    toAnyVisitor<float>(StreamOutputOp<float>()),
    toAnyVisitor<double>(StreamOutputOp<double>()),
    toAnyVisitor<std::string>(StreamOutputOp<std::string>()),
};

std::ostream& operator<<(std::ostream& os, const std::any& a)
{
    if (const auto it = codein::anyVisitors.find(std::type_index(a.type()));
        it != codein::anyVisitors.cend()) {
        return it->second(os, a);
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::any>& va)
{
    for (const auto& a: va) {
        os << a << " ";
    }

    return os;
}

template <typename T>
struct EqOp {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs == rhs;
    }
};

AnyBinaryComparerVisitorMap anyEqVisitors{
    toAnyEqVisitor<int>(EqOp<int>()),
    toAnyEqVisitor<uint>(EqOp<uint>()),
    toAnyEqVisitor<float>(EqOp<float>()),
    toAnyEqVisitor<double>(EqOp<double>()),
    toAnyEqVisitor<std::string>(EqOp<std::string>()),
};

bool operator==(const std::any& lhs, const std::any& rhs)
{
    auto ti = std::type_index(lhs.type());
    if (ti != std::type_index(rhs.type())) {
        return false;
    }

    const auto it = anyEqVisitors.find(ti);
    assert(it != anyEqVisitors.cend());

    return it->second(lhs, rhs);
}

}
