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
    toAnyVisitor<bool>(StreamOutputOp<bool>()),
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

AnyBinCompVisitorMap anyEqVisitors{
    toAnyBinCompVisitor<bool>(EqOp<bool>()),
    toAnyBinCompVisitor<int>(EqOp<int>()),
    toAnyBinCompVisitor<uint>(EqOp<uint>()),
    toAnyBinCompVisitor<float>(EqOp<float>()),
    toAnyBinCompVisitor<double>(EqOp<double>()),
    toAnyBinCompVisitor<std::string>(EqOp<std::string>()),
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

template <typename T>
struct LtOp {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};

AnyBinCompVisitorMap anyLtVisitors{
    toAnyBinCompVisitor<bool>(LtOp<bool>()),
    toAnyBinCompVisitor<int>(LtOp<int>()),
    toAnyBinCompVisitor<uint>(LtOp<uint>()),
    toAnyBinCompVisitor<float>(LtOp<float>()),
    toAnyBinCompVisitor<double>(LtOp<double>()),
    toAnyBinCompVisitor<std::string>(LtOp<std::string>()),
};

bool operator<(const std::any& lhs, const std::any& rhs)
{
    auto ti = std::type_index(lhs.type());
    if (ti != std::type_index(rhs.type())) {
        return false;
    }

    const auto it = anyLtVisitors.find(ti);
    assert(it != anyLtVisitors.cend());

    return it->second(lhs, rhs);
}

template <typename T>
struct GtOp {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs > rhs;
    }
};

template <typename T>
struct AddOp {
    T operator()(const T& lhs, const T& rhs) const {
        return lhs + rhs;
    }
};

AnyBinArithOpVisitorMap anyAddVisitors{
    toAnyBinArithOpVisitor<int>(AddOp<int>()),
    toAnyBinArithOpVisitor<uint>(AddOp<uint>()),
    toAnyBinArithOpVisitor<float>(AddOp<float>()),
    toAnyBinArithOpVisitor<double>(AddOp<double>()),
};

std::any operator+(const std::any& lhs, const std::any& rhs)
{
    auto ti = std::type_index(lhs.type());
    if (ti != std::type_index(rhs.type())) {
        return false;
    }

    const auto it = anyAddVisitors.find(ti);
    assert(it != anyAddVisitors.cend());

    return it->second(lhs, rhs);
}

}
