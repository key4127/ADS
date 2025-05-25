#ifndef LSM_KV_EMTABLE_H
#define LSM_KV_EMTABLE_H

#include <cstdint>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <limits>

#include "utils.h"
#include "threadpool.h"

struct DataBlock {
    uint64_t key;
    std::vector<float> vec;
};

class emtable {
private:
    uint64_t dimension = 768;
    std::vector<DataBlock> dataBlock;
    std::string pathDir;

    bool isVecDelete(std::vector<float> vec, ThreadPool *pool);

public:
    emtable();
    emtable(const char *path, ThreadPool *pool);

    uint64_t getDimention() {
        return dimension;
    }

    void putFile(const char *path);
    void loadFile(const char *path, ThreadPool *pool);

    void reset();
    void put(uint64_t key, std::vector<float> vec);
    void del(uint64_t key);
    std::vector<float> get(uint64_t key, ThreadPool *pool);

    std::vector<DataBlock> getDataBlock();
};

#endif