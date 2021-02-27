#include <any>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <utility>
#include <vector>

#include "metadata.h"
#include "iterator.h"
#include "csv_file_scanner.h"
#include "projector.h"
#include "to_any_converter.h"

template<class T, class F>
constexpr std::pair<
    const std::type_index,
    std::function<std::ostream& (std::ostream&, const std::any&)>
>
toAnyVisitor(F const &f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](std::ostream&os, std::any const &a) -> std::ostream&
        {
            static_assert(!std::is_void_v<T>);
            g(os, std::any_cast<T const&>(a));
            return os;
        }
    };
}
 
using AnyVisitorMap = std::unordered_map<
    std::type_index,
    std::function<std::ostream&(std::ostream&, const std::any&)>
>;

template<class T, class F>
inline void registerAnyVisitor(AnyVisitorMap& any_visitors, F const& f)
{
    std::cout << "Register visitor for type "
              << std::quoted(typeid(T).name()) << '\n';
    any_visitors.insert(toAnyVisitor<T>(f));
}
 
static AnyVisitorMap any_visitors {
    toAnyVisitor<int>([](std::ostream& os, int x){ os << x; }),
    toAnyVisitor<float>([](std::ostream& os, float x){ os << x; }),
    toAnyVisitor<std::string>([](std::ostream& os, std::string x){ os << x; }),
};

std::ostream& operator<<(std::ostream& os, const std::any& a)
{
    if (const auto it = any_visitors.find(std::type_index(a.type()));
        it != any_visitors.cend()) {
        return it->second(os, a);
    }

    return os;
}

using namespace std;
using namespace codein;

ostream& operator<<(ostream& os, const vector<any>& va)
{
    for (const auto& a: va) {
        os << a << " ";
    }

    return os;
}

int main()
{
    Metadata metadata{
        { "a", tiInt },
        { "b", tiFloat },
        { "c", tiString },
    };
    vector<string> lines{
        "1,1.1,Yoonsoo Kim",
        "2,2.2,Youngjun Kim",
        "3,3.3,Yeeun Kim",
    };

    auto scanner = makeIterator<CsvFileScanner>(metadata, lines);
    
    // Project only "a" & "c" columns.
    vector<string> columns{"a", "c"};
    // Combine the projector with the scanner.
    auto projector = makeIterator<Projector>(columns, std::move(scanner));
    projector->open();
    while (projector->hasMore()) {
        auto row = projector->processNext();
        if (row.has_value()) {
            cout << row.value() << "\n";
        }
    }
}
