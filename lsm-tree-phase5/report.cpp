#include <iostream>
#include <fstream>
#include <chrono>

#include "kvstore.h"
#include "embedding.h"

std::vector<std::string> read_file(std::string filename) {
	std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr<<"Failed to open file: "<<filename<<std::endl;
        return {};
    }
    std::string line;
    std::vector<std::string> temp;
    while (std::getline(file, line)) {
        bool exist_alpha = false;
        for (auto c : line) {
            if (isalpha(c)) {
                exist_alpha = true;
                break;
            }
        }
        if (!exist_alpha) {
            continue;
        }
        if (line.empty())
            continue;
        if(line.size() < 70) {
            continue;
        }
        temp.push_back(line);
        std::getline(file, line);
    }
    file.close();
    return temp;
}

int main()
{
    auto text = read_file("./data/cleaned_text_100k.txt");
    class KVStore store("./data");
    store.reset();

    const int max = 1000;

    for (int i = 0; i < max; i++) {
        store.put(i, text[i]);
    }

    store.testEmtable(max);
}