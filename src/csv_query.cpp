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
#include "any_visitor.h"

using namespace std;
using namespace codein;

int main()
{
    Metadata metadata{
        { "a", tiInt },
        { "b", tiFloat },
        { "c", tiString },
    };
    vector<string> lines{
        "1,1.1,John Smith",
        "2,2.2,Alex Smith",
        "3,3.3,Alex Swanson",
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
//            cout << row.value() << "\n";
        }
    }
}
