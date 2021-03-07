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
    toAnyVisitor<unsigned>(StreamOutputOp<unsigned>()),
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

using AnyBinCompVisitorMap = std::unordered_map<std::type_index, AnyBinComp>;

AnyBinCompVisitorMap anyEqVisitors{
    toAnyBinCompVisitor<bool>(std::equal_to<bool>()),
    toAnyBinCompVisitor<int>(std::equal_to<int>()),
    toAnyBinCompVisitor<unsigned>(std::equal_to<unsigned>()),
    toAnyBinCompVisitor<float>(std::equal_to<float>()),
    toAnyBinCompVisitor<double>(std::equal_to<double>()),
    toAnyBinCompVisitor<std::string>(std::equal_to<std::string>()),
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

AnyBinCompVisitorMap anyLtVisitors{
    toAnyBinCompVisitor<bool>(std::less<bool>()),
    toAnyBinCompVisitor<int>(std::less<int>()),
    toAnyBinCompVisitor<unsigned>(std::less<unsigned>()),
    toAnyBinCompVisitor<float>(std::less<float>()),
    toAnyBinCompVisitor<double>(std::less<double>()),
    toAnyBinCompVisitor<std::string>(std::less<std::string>()),
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

AnyBinCompVisitorMap anyLteVisitors{
    toAnyBinCompVisitor<bool>(std::less_equal<bool>()),
    toAnyBinCompVisitor<int>(std::less_equal<int>()),
    toAnyBinCompVisitor<unsigned>(std::less_equal<unsigned>()),
    toAnyBinCompVisitor<float>(std::less_equal<float>()),
    toAnyBinCompVisitor<double>(std::less_equal<double>()),
    toAnyBinCompVisitor<std::string>(std::less_equal<std::string>()),
};

bool operator<=(const std::any& lhs, const std::any& rhs)
{
    auto ti = std::type_index(lhs.type());
    if (ti != std::type_index(rhs.type())) {
        return false;
    }

    const auto it = anyLteVisitors.find(ti);
    assert(it != anyLteVisitors.cend());

    return it->second(lhs, rhs);
}

AnyBinCompVisitorMap anyGtVisitors{
    toAnyBinCompVisitor<int>(std::greater<int>()),
    toAnyBinCompVisitor<unsigned>(std::greater<unsigned>()),
    toAnyBinCompVisitor<float>(std::greater<float>()),
    toAnyBinCompVisitor<double>(std::greater<double>()),
    toAnyBinCompVisitor<std::string>(std::greater<std::string>()),
};

bool operator>(const std::any& lhs, const std::any& rhs)
{
    auto ti = std::type_index(lhs.type());
    if (ti != std::type_index(rhs.type())) {
        return false;
    }

    const auto it = anyGtVisitors.find(ti);
    assert(it != anyGtVisitors.cend());

    return it->second(lhs, rhs);
}

AnyBinCompVisitorMap anyGteVisitors{
    toAnyBinCompVisitor<int>(std::greater_equal<int>()),
    toAnyBinCompVisitor<unsigned>(std::greater_equal<unsigned>()),
    toAnyBinCompVisitor<float>(std::greater_equal<float>()),
    toAnyBinCompVisitor<double>(std::greater_equal<double>()),
    toAnyBinCompVisitor<std::string>(std::greater_equal<std::string>()),
};

bool operator>=(const std::any& lhs, const std::any& rhs)
{
    auto ti = std::type_index(lhs.type());
    if (ti != std::type_index(rhs.type())) {
        return false;
    }

    const auto it = anyGteVisitors.find(ti);
    assert(it != anyGteVisitors.cend());

    return it->second(lhs, rhs);
}

using AnyBinArithOpVisitorMap = std::unordered_map<std::type_index, AnyBinArithOp>;

template <typename T>
struct AddOp {
    T operator()(const T& lhs, const T& rhs) const {
        return lhs + rhs;
    }
};

AnyBinArithOpVisitorMap anyAddVisitors{
    toAnyBinArithOpVisitor<int>(AddOp<int>()),
    toAnyBinArithOpVisitor<unsigned>(AddOp<unsigned>()),
    toAnyBinArithOpVisitor<float>(AddOp<float>()),
    toAnyBinArithOpVisitor<double>(AddOp<double>()),
    toAnyBinArithOpVisitor<std::string>(AddOp<std::string>()),
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
