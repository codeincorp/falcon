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
#include "to_any_converter.h"

template<class T, class F>
constexpr std::pair<
    const std::type_index,
    std::function<std::ostream& (std::ostream&, const std::any&)>
>
to_any_visitor(F const &f)
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
 
using any_visitor_map = std::unordered_map<
    std::type_index,
    std::function<std::ostream&(std::ostream&, const std::any&)>
>;

template<class T, class F>
inline void register_any_visitor(any_visitor_map& any_visitors, F const& f)
{
    std::cout << "Register visitor for type "
              << std::quoted(typeid(T).name()) << '\n';
    any_visitors.insert(to_any_visitor<T>(f));
}
 
static any_visitor_map any_visitors {
    to_any_visitor<int>([](std::ostream& os, int x){ os << x; }),
    to_any_visitor<float>([](std::ostream& os, float x){ os << x; }),
    to_any_visitor<std::string>([](std::ostream& os, std::string x){ os << x; }),
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
        { "a", int_ti },
        { "b", float_ti },
        { "c", string_ti },
    };
    vector<string> lines{
        "1,1.1,Yoonsoo Kim",
        "2,2.2,Youngjun Kim",
        "3,3.3,Yeeun Kim",
    };

    auto scanner = make_iterator<CsvFileScanner>(metadata, lines);
    scanner->open();
    while (scanner->hasMore()) {
        auto row = scanner->processNext();
        if (row.has_value()) {
            cout << row.value() << "\n";
        }
    }
}
