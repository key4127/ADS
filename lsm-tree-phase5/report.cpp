#include <iostream>
#include <fstream>
#include <chrono>

#include "kvstore.h"
#include "embedding.h"

//extern std::chrono::milliseconds tokenize_duration, initialize_duration, batch_duration;

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

void embedding_output()
{
    //std::cout << "Tokenize elapsed time: " << tokenize_duration.count() << " milliseconds" << std::endl;
    //std::cout << "Intialize elapsed time: " << initialize_duration.count() << " milliseconds" << std::endl;
    //std::cout << "Batch elapsed time: " << batch_duration.count() << " milliseconds" << std::endl;
}

int main()
{
    auto trimmed_text = read_file("./data/trimmed_text.txt");
    auto test_text = read_file("./data/test_text.txt");
    auto ans = read_file("./data/test_text_ans.txt");
    class KVStore store("./data");
    store.reset();

    const uint64_t max = 120;

    for (int i = 0; i < max; i++) {
        store.put(i, trimmed_text[i]);
    }

    for (int i = 0; i < max; i++) {
        std::vector<std::pair<std::uint64_t, std::string>> result =
            store.search_knn(trimmed_text[i], 1);
        if (result[0].second != trimmed_text[i]) {
            std::cout << "Error: value[" << i << "] is not correct" << std::endl;
            //std::cout << result[0].second << "\n";
            //std::cout << trimmed_text[i] << "\n";
        }
    }
}