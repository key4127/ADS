#ifndef HNSW_H
#define HNSW_H

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <random>
#include <chrono>
#include <iostream>

#include "embedding.h"
#include "utils.h"
#include "emtable.h"
#include "threadpool.h"

static uint32_t m_M;
static uint32_t m_L;
static uint32_t efConstruction;
static uint32_t M_max;

class HWnode {
public:
    uint32_t id;
    uint32_t level;
    uint64_t key;
    std::vector<float> vec;
    std::vector<std::vector<uint32_t>> next;

    HWnode() { }

    HWnode(uint64_t key, std::vector<float> vec) {
        this->key = key;
        this->vec = vec;
        next.resize(m_L + 1);
    }
};

struct HNbest { // the best at the top
    bool operator()(std::pair<HWnode*, float> x, std::pair<HWnode*, float> y) {
        if (x.second == y.second) {
            return x.first->key < y.first->key;
        }
        return x.second < y.second;
    }
};

struct HNworst {
    bool operator()(std::pair<HWnode*, float> x, std::pair<HWnode*, float> y) {
        if (x.second == y.second) {
            return x.first->key > y.first->key;
        }
        return x.second > y.second;
    }
};

class HNSW {
private:
    uint32_t dimension;
    uint32_t totalLevel;
    uint32_t nodeNum;
    std::vector<HWnode *> nodes; // save all nodes
    std::vector<HWnode *> head;
    std::unordered_map<uint32_t, std::vector<float>> deleted; // id and vec

    int randLevel();
    std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> searchLayer(
        std::vector<float> vec, HWnode* entry, int layer, int num);
    bool isDeleted(uint32_t id, std::vector<float> vec);

    void putNode(const std::string &path, int k);
    void loadNode(const std::string &path, const std::vector<DataBlock> &data, int k, std::vector<HWnode *> &tmpNodes);

public:
    HNSW();

    void insert(uint64_t key, std::vector<float> vec);
    void del(uint64_t key, std::vector<float> vec);
    std::vector<uint64_t> query(std::vector<float> vec, int k);
    void reset();

    void putFile(const std::string &path, ThreadPool *pool);
    void loadFile(const std::string &path, const std::vector<DataBlock> &data, ThreadPool *pool);
};

#endif