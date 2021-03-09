#include <any>
#include <cassert>
#include <functional>
#include <string>
#include <typeindex>

#include "any_visitor.h"
#include "to_any_converter.h"

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

inline bool apply(const AnyBinCompVisitorMap& opMap, const std::any& lhs, const std::any& rhs)
{
    auto ti = std::type_index(lhs.type());
    if (ti != std::type_index(rhs.type())) {
        return false;
    }
    const auto it = opMap.find(ti);
    assert(it != opMap.cend());

    return it->second(lhs, rhs);
}

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
    if(lhs.type() == typeid(void) && rhs.type() == typeid(void)) {
        return true;
    }
    
    return apply(anyEqVisitors, lhs, rhs);
}

bool operator!=(const std::any& lhs, const std::any& rhs)
{
    return !(lhs == rhs);
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
    return apply(anyLtVisitors, lhs, rhs);
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
    return apply(anyLteVisitors, lhs, rhs);
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
    return apply(anyGtVisitors, lhs, rhs);
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
    return apply(anyGteVisitors, lhs, rhs);
}

using AnyBinArithOpVisitorMap = std::unordered_map<std::type_index, AnyBinArithOp>;

inline std::any apply(const AnyBinArithOpVisitorMap& opMap, const std::any& lhs, const std::any& rhs)
{
    auto ti = std::type_index(lhs.type());
    if (ti != std::type_index(rhs.type())) {
        return std::any();
    }

    const auto it = opMap.find(ti);
    assert(it != opMap.cend());

    return it->second(lhs, rhs);
}

AnyBinArithOpVisitorMap anyAddVisitors{
    toAnyBinArithOpVisitor<int>(std::plus<int>()),
    toAnyBinArithOpVisitor<unsigned>(std::plus<unsigned>()),
    toAnyBinArithOpVisitor<float>(std::plus<float>()),
    toAnyBinArithOpVisitor<double>(std::plus<double>()),
    toAnyBinArithOpVisitor<std::string>(std::plus<std::string>()),
};

std::any operator+(const std::any& lhs, const std::any& rhs)
{
    return apply(anyAddVisitors, lhs, rhs);
}

AnyBinArithOpVisitorMap anySubVisitors{
    toAnyBinArithOpVisitor<int>(std::minus<int>()),
    toAnyBinArithOpVisitor<unsigned>(std::minus<unsigned>()),
    toAnyBinArithOpVisitor<float>(std::minus<float>()),
    toAnyBinArithOpVisitor<double>(std::minus<double>()),
};

std::any operator-(const std::any& lhs, const std::any& rhs)
{
    return apply(anySubVisitors, lhs, rhs);
}

AnyBinArithOpVisitorMap anyMultVisitors{
    toAnyBinArithOpVisitor<int>(std::multiplies<int>()),
    toAnyBinArithOpVisitor<unsigned>(std::multiplies<unsigned>()),
    toAnyBinArithOpVisitor<float>(std::multiplies<float>()),
    toAnyBinArithOpVisitor<double>(std::multiplies<double>()),
};

std::any operator*(const std::any& lhs, const std::any& rhs)
{
    return apply(anyMultVisitors, lhs, rhs);
}

AnyBinArithOpVisitorMap anyDivVisitors{
    toAnyBinArithOpVisitor<int>(std::divides<int>()),
    toAnyBinArithOpVisitor<unsigned>(std::divides<unsigned>()),
    toAnyBinArithOpVisitor<float>(std::divides<float>()),
    toAnyBinArithOpVisitor<double>(std::divides<double>()),
};

std::any operator/(const std::any& lhs, const std::any& rhs)
{
    return apply(anyDivVisitors, lhs, rhs);
}

AnyBinArithOpVisitorMap anyModVisitors{
    toAnyBinArithOpVisitor<int>(std::modulus<int>()),
    toAnyBinArithOpVisitor<unsigned>(std::modulus<unsigned>()),
};

std::any operator%(const std::any& lhs, const std::any& rhs)
{
    return apply(anyModVisitors, lhs, rhs);
}

}
