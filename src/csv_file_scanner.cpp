#include <cassert>
#include <optional>
#include <string>
#include <vector>

#include "metadata.h"
#include "iterator.h"
#include "csv_file_scanner.h"
#include "to_any_converter.h"

namespace codein {

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
        r.emplace_back(convertTo(anyConverters, metadata_[i].typeIndex, fields[i]));
    }

    return std::move(r);
};

} // namespace codein
