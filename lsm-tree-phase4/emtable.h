#ifndef LSM_KV_EMTABLE_H
#define LSM_KV_EMTABLE_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <limits>

#include "utils.h"

struct DataBlock {
    uint64_t key;
    std::vector<float> vec;
};

class emtable {
private:
    uint64_t dimension = 768;
    std::vector<DataBlock> dataBlock;
    std::string pathDir;

public:
    emtable();
    emtable(const char *path);

    uint64_t getDimention() {
        return dimension;
    }

    void putFile(const char *path);
    void loadFile(const char *path);

    void reset();
    void put(uint64_t key, std::vector<float> vec);
    void del(uint64_t key);

    std::vector<DataBlock> getDataBlock();
};

#endif