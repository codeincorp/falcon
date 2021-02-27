#include <any>
#include <string>
#include <typeindex>

#include "any_visitor.h"

namespace codein {

AnyVisitorMap anyVisitors {
    toAnyVisitor<int>([](std::ostream& os, int x){ os << x; }),
    toAnyVisitor<float>([](std::ostream& os, float x){ os << x; }),
    toAnyVisitor<std::string>([](std::ostream& os, std::string x){ os << x; }),
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

}
