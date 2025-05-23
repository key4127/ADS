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
    }
    file.close();
    return temp;
}

int main()
{
    auto text = read_file("./data/cleaned_text_100k.txt");
    class KVStore store("./data");
    store.reset();

    const uint64_t max = 100;

    for (int i = 0; i < max; i++) {
        store.put(i, text[2 * i + 1]);
    }

    for (int i = 0; i < max; i++) {
        std::vector<std::pair<std::uint64_t, std::string>> result =
            store.search_knn(text[2 * i + 1], 1);
        if (result[0].second != text[2 * i + 1]) {
            std::cout << "Error: value[" << i << "] is not correct" << std::endl;
        }
        store.del(i);
        result = store.search_knn(text[2 * i + 1], 1);
        if (result.size() >= 1 && result[0].second == text[2 * i + 1]) {
            std::cout << "Error: value[" << i << "] is not correct" << std::endl;
        }
    }

    

    //auto end = std::chrono::high_resolution_clock::now();
    //std::cout << "Total cost: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "\n";

    //store.output();
}