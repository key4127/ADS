#include "HNSW.h"
#include "embedding.h"

int main()
{
    HNSW h;
    std::vector<std::string> words;
    words.push_back("apple");
    words.push_back("banana");
    words.push_back("man");
    std::string query = "fruit";

    for (int i = 0; i < words.size(); i++) {
        std::vector<float> vec = embedding(words[i])[0];
        HWType type = NORMAL;
        HWnode *node = new HWnode(i, vec, type);
    }
}