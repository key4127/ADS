#pragma once
#ifndef READ_EMBEDDING_H
#define READ_EMBEDDING_H

#include <iostream>
#include <vector>
#include <fstream>

#include "utils.h"

static const std::string valPath = "./data/cleaned_text_100k.txt";
static const std::string vecPath = "./data/embedding_100k.txt";

std::string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(" \t\n\r\f\v");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(first, last - first + 1);
}

std::vector<float> getVec(std::string val, int n)
{
    std::ifstream valFile(valPath);
    std::ifstream vecFile(vecPath);

    std::string valLine;
    std::string vecLine;
    while (std::getline(valFile, valLine)) {
        if (trim(valLine) == trim(val)) {
            //std::cout << "to find: " << val << std::endl;
            //std::cout << "find: " << valLine << std::endl;

            std::getline(vecFile, vecLine);

            vecLine = trim(vecLine);
            vecLine = vecLine.substr(1, vecLine.length() - 2);
            
            std::vector<float> vec;
            int start = 0, end = 0;
            for (int i = 0; i < n - 1; i++) {
                end = vecLine.find(',', start);
                vec.push_back(std::stof(vecLine.substr(start, end - start)));
                start = end + 2;
            }
            vec.push_back(std::stof(vecLine.substr(start)));

            return vec;

        } else {
            std::getline(valFile, valLine);
            std::getline(vecFile, vecLine);
            std::getline(vecFile, vecLine);
        }
    }

    std::vector<float> vec;
    for (int i = 0; i < n; i++) {
        vec.push_back(std::numeric_limits<float>::max());
    }
    return vec;
}

#endif