#include <type_traits>
#include <any>
#include <functional>
#include <iomanip>
#include <iostream>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <set>
#include <vector>
#include <string>
#include <list>
#include <array>
#include <memory>
 
template<class T, class F>
constexpr std::pair<const std::type_index, std::function<void(std::any const&)>>
    to_any_visitor(F const &f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](std::any const &a)
        {
            if constexpr (std::is_void_v<T>) {
                g();
            } else {
                g(std::any_cast<T const&>(a));
            }
        }
    };
}
 
using any_visitor_map = std::unordered_map<
    std::type_index, std::function<void(std::any const&)>>;

inline void process(const any_visitor_map& any_visitor, const std::any& a)
{
    if (const auto it = any_visitor.find(std::type_index(a.type()));
        it != any_visitor.cend()) {
        it->second(a);
    } else {
        std::cout << "Unregistered type "<< std::quoted(a.type().name());
    }
}
 
template<class T, class F>
    inline void register_any_visitor(any_visitor_map& any_visitor, F const& f)
{
    std::cout << "Register visitor for type "
              << std::quoted(typeid(T).name()) << '\n';
    any_visitor.insert(to_any_visitor<T>(f));
}
 
struct my_type {
    std::string s;
    std::vector<int> vi;
};

auto main() -> int
{
    static any_visitor_map any_visitor {
        to_any_visitor<void>([]{ std::cout << "{}"; }),
        to_any_visitor<int>([](int x){ std::cout << x; }),
        to_any_visitor<unsigned>([](unsigned x){ std::cout << x; }),
        to_any_visitor<float>([](float x){ std::cout << x; }),
        to_any_visitor<double>([](double x){ std::cout << x; }),
        to_any_visitor<char const*>([](char const *s)
            { std::cout << std::quoted(s); }),
        to_any_visitor<std::string>([](std::string x){ std::cout << std::quoted(x); })
        // ... add more handlers for your types ...
    };
 
    std::vector<std::any> va { {}, 42, 123u, 3.14159f, 2.71828, "C++17", std::string("C++20")};
 
    std::cout << "{ ";
    for (const std::any& a : va) {
        process(any_visitor, a);
        std::cout << ", ";
    }
    std::cout << "}\n";
 
    process(any_visitor, std::any(0xFULL)); //< Unregistered type "y" (unsigned long long)
    std::cout << '\n';
 
    register_any_visitor<unsigned long long>(any_visitor, [](auto x) {
        std::cout << std::hex << std::showbase << x; 
        std::cout << std::dec << std::noshowbase;
    });
 
    process(any_visitor, std::any(0xFULL)); //< OK: 0xf
    std::cout << '\n';

    std::optional opt_str = "abc";

    std::cout << opt_str.value() << std::endl;

    std::optional<std::vector<std::any>> opt_vec = va;
    std::cout << "{ ";
    for (const std::any& a : opt_vec.value()) {
        process(any_visitor, a);
        std::cout << ", ";
    }
    std::cout << "}\n";

    std::optional<int> opt_int = std::nullopt;
    std::cout << opt_int.value_or(-1) << std::endl;

    std::unordered_map<std::string, int> int_map{{"1",1}, {"2",2}};

    std::set int_set{1,2,3,4};

    std::string s{"std::string"};

    std::list l{1,2,3};

    std::optional<std::string> opt = std::nullopt;

    my_type mt{"ABCD", {3,2,1}};

    std::array arr{1,2,3,4,5};

    auto p = std::make_unique<my_type>(std::string{"EFGH"}, std::vector{4,5,6});

    auto p1 = std::make_shared<my_type>(std::string{"EFGH"}, std::vector{4,5,6});

    return 0;
}
