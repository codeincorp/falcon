/**
 * Copyright (C) 2021-present Codein Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of
 * BSD-3-Clause License which can be found at the root directory of this repository.
 */

#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "any_visitor.h"
#include "csv_file_scanner.h"
#include "iterator.h"
#include "metadata.h"
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
            strs.emplace_back(line.substr(left, right - left));
            left = 0;
            right = 0;
        }
        else if (line.at(i) != ' ' && line.at(i) != '\t') {
            if (right == 0) {
                left = i;
            }
            right = i+1;
        }
        
        i++;
    }
    return strs;
}

Metadata parseLineMetadata(const std::string& line)
{
    std::vector<std::string> fieldsMetadata = parseLine(line);
    Metadata reVec;
    reVec.reserve(fieldsMetadata.size());
    size_t indexOfSeparator = 0;
    size_t right = 0;

    for (size_t i = 0; i < fieldsMetadata.size(); ++i) {
        // parseLine already trims extra spaces on both sides
        indexOfSeparator = fieldsMetadata[i].find('/');
        // if an lnvalid line does not contain slash or field name or empty string
        if (indexOfSeparator == std::string::npos || indexOfSeparator == 0) {
            throw InvalidMetadata();
        }

        right = fieldsMetadata[i].find_first_not_of(" \t", indexOfSeparator + 1);
        // if type name is empty
        if (right == std::string::npos) {
            throw InvalidMetadata();
        }
        auto typeIndex = convertToTypeid(
            fieldsMetadata[i].substr(right, fieldsMetadata[i].size() - right));
        // if type name is invalid
        if (typeIndex == tiVoid) {
            throw InvalidMetadata();
        }
        
        reVec.emplace_back(
            fieldsMetadata[i].substr(0, fieldsMetadata[i].find_last_not_of(" \t", indexOfSeparator - 1) + 1),
            typeIndex
        );
    }
    
    return reVec;
}

void CsvFileScanner::constructorHelper(const std::string& metadataFileName, const std::string& dataFileName) 
{
    std::fstream mfs(metadataFileName);
    if (!mfs.is_open()) {
        throw NonExistentFile();
    }
    std::string reading;
    std::getline(mfs, reading);
    metadata_ = parseLineMetadata(reading);

    dfs_.open(dataFileName);
    if (!dfs_.is_open()) {
        throw NonExistentFile();
    }
}

CsvFileScanner::CsvFileScanner(
    const std::string& metadataFileName,
    const std::string& dataFileName,
    const Expression& filterExpr)
    : metadata_()
    , dfs_()
    , filterExpr_(filterExpr)
    , readLines_(0)
    , errorLines_(0)
{
    constructorHelper(metadataFileName, dataFileName);

    for (size_t i = 0; i < metadata_.size(); ++i) {
        projections_.emplace_back(OpCode::Ref, metadata_[i].fieldName);
    }
}

CsvFileScanner::CsvFileScanner(
    const std::string& metadataFileName,
    const std::string& dataFileName,
    const Expression& filterExpr,
    const std::vector<Expression>& projections)
    : metadata_()
    , dfs_()
    , filterExpr_(filterExpr)
    , readLines_(0)
    , errorLines_(0)
    , projections_(projections)
{
    constructorHelper(metadataFileName, dataFileName);
}

void CsvFileScanner::checkError() 
{
    /* current convertTo throws nullany only  when non-numeric string attempts to be converted into
     * numeric type such as double, int, etc. thus, it doesn't work when a numeric type, such as double
     * attempts to be converted into another numeric type such as int, causing only lossy conversion
     * We ignore data if it does not match to metadata description
     * If we've already read more than kThreshold lines and number of error lines is greater than half 
     * of number of read lines, then notify it and give up further processing, 
     * assuming that probably the wrong metadata is specified.  
     */
    if (readLines_ > kThreshold && errorLines_ > readLines_ / 2) {
        std::cerr << "Too many discrepencies between specified metadata and actual data lines: "
                  << "\nInvalid metadata: aborting process\n"
                  << "read lines: " << readLines_ << "\nerror lines: "<< errorLines_ << std::endl;
        errorLines_ = 0;
        throw WrongMetadata();
    }
}

std::optional<std::vector<std::any>> CsvFileScanner::processNext()
{
    if (!hasNext()) {
        return std::nullopt;
    }

    std::vector<std::any> r;
    std::string line;
    while (r.empty()) {
        std::getline(dfs_, line);
        auto fields = parseLine(line);
        
        if (dfs_.fail()) {
            if (errorLines_ != 0) {
                std::cerr << "There were some discrepencies between metadata and actual data lines: \n"
                          << "read lines: " << readLines_ << "\nerror lines " << errorLines_ << std::endl;
            }

            return std::nullopt;
        }
        ++readLines_;

        if (fields.size() != metadata_.size()) {
            ++errorLines_;

            checkError();

            continue;
        }

        for (size_t i = 0; i < metadata_.size(); ++i) {
            const auto field = convertTo(metadata_[i].typeIndex, fields[i]);
            if (!field.has_value()) {
                ++errorLines_;

                checkError();

                r.clear();
                break;
            }

            r.emplace_back(field);
        }

        auto hasFilterPassed = filterExpr_.eval(metadata_, r);
        if (notAny(hasFilterPassed)) {
            r.clear();
        }
    }

    size_t size = projections_.size();
    std::vector<std::any> output;
    output.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        output.emplace_back(std::move(projections_[i].eval(metadata_, r)));
    }

    return std::move(output);
}

} // namespace codein
