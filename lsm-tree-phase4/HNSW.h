#ifndef HNSW_H
#define HNSW_H

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <vector>
#include <queue>
#include <unordered_set>
#include <cmath>
#include <random>
#include <chrono>
#include <iostream>

#include "embedding.h"

static int m_M;
static int m_L;
static int efConstruction;
static int M_max;

class HWnode {
public:
    uint64_t key;
    std::vector<float> vec;
    std::vector<std::vector<HWnode *>> next;

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
    int totalLevel;
    std::vector<HWnode *> head;

    int randLevel();
    std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> searchLayer(
        std::vector<float> vec, HWnode* entry, int layer, int num);

public:
    HNSW();

    void insert(uint64_t key, std::vector<float> vec);
    std::vector<uint64_t> query(std::vector<float> vec, int k);
};

#endif