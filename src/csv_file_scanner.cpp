#include <cassert>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

#include "metadata.h"
#include "iterator.h"
#include "csv_file_scanner.h"
#include "to_any_converter.h"

namespace codein {

std::vector<std::string> parseLine(const std::string& line)
{
    int left = 0;
    int right = 0;
    int len = line.length();
    int i = 0;
    std::vector<std::string> strs;

    while (i <= len) {
        if (i == len || line.at(i) == ','){
            strs.emplace_back(line.substr(left, right-left));
            left = 0;
            right = 0;
        }
        else if (line.at(i) != ' ') {
            if (right == 0) {
                left = i;
            }
            right = i+1;
        }
        
        i++;
    }
    return strs;
}

std::type_index convertToTypeid(const std::string& typeName)
{
    if(typeName == "int") {
        return tiInt;
    }
    else if (typeName == "float") {
        return tiFloat;
    }
    else if (typeName == "uint") {
        return tiUint;
    }
    else if (typeName == "double") {
        return tiDouble;
    }
    else if (typeName == "string") {
        return tiString;
    }
    else {
        return tiVoid;
    }
}

Metadata parseLineMetadata(const std::string& line)
{
    std::vector<std::string> temp = parseLine(line);
    Metadata reVec;
    reVec.reserve(temp.size());
    size_t indexOfSeparator = 0;
    size_t right = 0;

    for (size_t i = 0; i < temp.size(); ++i) {
        //parseLine already trims extra spaces on both sides
        indexOfSeparator = temp[i].find('/');
        //if an lnvalid line does not contain slash or field name or empty string
        if (indexOfSeparator == std::string::npos ||
            indexOfSeparator == 0 ) {
            throw InvalidMetadata();
        }

        right = temp[i].find_first_not_of(' ', indexOfSeparator+1);
        //empty type name
        if(right == std::string::npos) {
            throw InvalidMetadata();
        }
        auto typeIndex = convertToTypeid(temp[i].substr(right, temp[i].size()-right));
        //if invalid type name
        if(typeIndex == tiVoid) {
            throw InvalidMetadata();
        }
        
        reVec.emplace_back(
            temp[i].substr(0, temp[i].find_last_not_of(' ', indexOfSeparator-1)+1),
            typeIndex
        );
    }
    
    return reVec;
}

CsvFileScanner::CsvFileScanner(const std::string& metadataFileName, const std::string& dataFileName)
    : metadata_()
    , lines_()
    , it_()
{
    std::fstream mfs(metadataFileName);
    std::string reading;
    std::getline(mfs, reading);
    metadata_ = parseLineMetadata(reading);

    // initialize metadata_ from mfs

    std::fstream dfs(dataFileName);
    
    while (!dfs.eof()) {
        getline(dfs, reading);
        lines_.emplace_back(reading);
    }

    it_ = lines_.cend();
    // initialize lines_
}

std::optional<std::vector<std::any>> CsvFileScanner::processNext()
{
    if (!hasMore()) {
        return std::nullopt;
    }

    auto fields = parseLine(*it_);
    ++it_;

    assert(fields.size() == metadata_.size());

    std::vector<std::any> r;
    for (size_t i = 0; i < metadata_.size(); ++i) {
        r.emplace_back(convertTo(anyConverters, metadata_[i].typeIndex, fields[i]));
    }

    return std::move(r);
};

} // namespace codein
