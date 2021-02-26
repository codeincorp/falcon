#include <any>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <functional>
#include <memory>
#include <optional>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

std::vector<std::string> parse_line(const std::string& line) {
    int left = -1;
    int right = 0;
    int len = line.length();
    int i = 0;
    std::vector<std::string> strs;

    while(i <= len) {
        if( i == len || line.at(i) == ','){
          strs.emplace_back(line.substr(left, right-left));  
          left = -1;      
        }
        else if (line.at(i) != ' '){
          if (left == -1){
            left = i;
          }
          right = i+1;
        }
        
        i++;
    }
    return strs;
}

struct FieldMetaInfo {
    std::string fieldName;
    std::type_index typeIndex;
};

using Metadata = std::vector<FieldMetaInfo>;

struct Iterator {
    virtual void open() = 0;
    virtual void reopen() = 0;
    virtual bool hasMore() const = 0;
    virtual std::optional<std::vector<std::any>> processNext() = 0;
    virtual void close() = 0;
    virtual const Metadata& getMetadata() const = 0;
    virtual ~Iterator() {}
};

template <typename T, typename... Args_>
static std::unique_ptr<Iterator> make_iterator(Args_&... args)
{
    return std::unique_ptr<Iterator>{new T(std::forward<Args_>(args)...)};
}

class CsvFileScanner : public Iterator {
public:
    template <typename T, typename... Args_>
    friend std::unique_ptr<Iterator>
    make_iterator(Args_&...);

    void open() override
    {
        it_ = lines_.cbegin();
    };

    void reopen() override {};

    bool hasMore() const override
    {
        return it_ != lines_.cend();
    }

    std::optional<std::vector<std::any>> processNext() override;

    void close() override
    {
        it_ = lines_.cend();
    };

    const Metadata& getMetadata() const override
    {
        return metadata_;
    };

    ~CsvFileScanner() override
    {
        it_ = lines_.cend();
    };

private:
    CsvFileScanner(const Metadata& metadata, const std::vector<std::string>& lines)
        : metadata_(metadata)
        , lines_(lines)
        , it_(lines_.cend())
    {};

    Metadata metadata_;
    std::vector<std::string> lines_;
    std::vector<std::string>::const_iterator it_;
};

template<class T>
constexpr std::pair<const std::type_index, std::function<std::any(const std::string&)>>
    to_any_converter(const std::function<T(const std::string&)> f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](const std::string& s) -> std::any
        {
            return g(s);
        }
    };
}
 
using any_converter_map = std::unordered_map<
    std::type_index,
    std::function<std::any(const std::string&)>
>;

inline std::any convert_to(const any_converter_map& any_converters, const std::type_index& ti, const std::string& s)
{
    if (const auto it = any_converters.find(ti); it != any_converters.cend()) {
        return it->second(s);
    }

    assert(!"unsupported type");

    return std::any();
}
 
static any_converter_map any_converters {
    to_any_converter<int>([](const std::string& s){ return stoi(s); }),
    to_any_converter<unsigned>([](const std::string& s){ return static_cast<unsigned>(stoul(s)); }),
    to_any_converter<float>([](const std::string& s){ return stof(s); }),
    to_any_converter<double>([](const std::string& s){ return stod(s); }),
    to_any_converter<std::string>([](const std::string& s){ return s; }),
};

const auto int_ti = std::type_index(typeid(int));
const auto float_ti = std::type_index(typeid(float));
const auto string_ti = std::type_index(typeid(std::string));
const auto double_ti = std::type_index(typeid(double));

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

std::optional<std::vector<std::any>> CsvFileScanner::processNext()
{
    if (!hasMore()) {
        return std::nullopt;
    }

    auto fields = parse_line(*it_);
    ++it_;

    assert(fields.size() == metadata_.size());

    std::vector<std::any> r;
    for (size_t i = 0; i < metadata_.size(); ++i) {
        r.emplace_back(convert_to(any_converters, metadata_[i].typeIndex, fields[i]));
    }

    return std::move(r);
};

using namespace std;

ostream& operator<<(ostream& os, const vector<any>& va)
{
    for (const auto& a: va) {
        os << a << " ";
    }

    return os;
}

int main()
{
    vector<string> vs{"1", "1.1", "Hello, there!", "3.141592"};
    vector<type_index> vt{int_ti, float_ti, string_ti, double_ti};

    register_any_visitor<double>(any_visitors, [](std::ostream& os, double x) { os << x; });

    for (int i = 0; i < vs.size(); ++i) {
        cout << std::quoted(vs[i]) << ": " << convert_to(any_converters, vt[i], vs[i]) << '\n';
    }

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
