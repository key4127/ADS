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

    const uint64_t max = 1000;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < max; i++) {
        if ((i + 1) % 1000 == 0) {
            std::cout << "Put 500 items.\n";
        }
        store.put(i, text[2 * i + 1]);
    }

    for (int i = 0; i < max; i++) {
        if ((i + 1) % 1000 == 0) {
            std::cout << "Get 500 items.\n";
        }
        if (store.get(i) != text[2 * i + 1]) {
            std::cout << "error: " << i << " " <<store.get(i) << std::endl;
        }
        store.del(i);
        if (store.get(i) != "") {
            std::cout << "error: " << i << std::endl;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "cost " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << "ms\n";

    /*for (int i = 0; i < max; i++) {
        std::vector<std::pair<std::uint64_t, std::string>> result =
            store.search_knn(text[i], 1);
        if (result[0].second != text[i]) {
            std::cout << "Error: value[" << i << "] is not correct" << std::endl;
        }
    }*/
}