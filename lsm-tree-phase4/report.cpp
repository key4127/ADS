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

    int k = 3;
    int knn_idx = 0, hnsw_idx = 0;
    int idx = 0;

    for (int i = 0; i < max; i++) {
        auto knn_res = store.search_knn(test_text[i], k);
        auto hnsw_res = store.search_knn_hnsw(test_text[i], k);

        for (int j = 0; j < k; j++) {
            /*if (knn_res[j].second == ans[idx]) {
                knn_idx++;
            }*/
            if (hnsw_res[j].second == ans[idx]) {
                hnsw_idx++;
            }
            idx++;
        }
    }

    printf("\n");
    printf("rate: %d/%d, %.3lf\n", hnsw_idx, 360, static_cast<double>(hnsw_idx) / 360);

    store.output();
}