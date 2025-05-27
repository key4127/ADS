#pragma once

#include "kvstore_api.h"
#include "skiplist.h"
#include "HNSW.h"
#include "sstable.h"
#include "sstablehead.h"
#include "emtable.h"
#include "threadpool.h"
#include <chrono>
#include <iostream>

#include <map>
#include <set>
#include <unordered_map>

class KVStore : public KVStoreAPI {
    // You can add your implementation here
private:
    skiplist *s = new skiplist(0.5); // memtable
    // std::vector<sstablehead> sstableIndex;  // sstable的表头缓存

    std::vector<sstablehead> sstableIndex[15]; // the sshead for each level
    emtable e;

    int totalLevel = -1; // 层数

    HNSW *h = new HNSW();

    std::chrono::microseconds skipInsertDuration = std::chrono::microseconds(0);
    std::chrono::microseconds skipDeleteDuration = std::chrono::microseconds(0);
    std::chrono::microseconds skipGetDuration = std::chrono::microseconds(0);
    std::chrono::microseconds compactionDuration = std::chrono::microseconds(0);
    std::chrono::microseconds readDuration = std::chrono::microseconds(0);
    std::chrono::microseconds putFileDuration = std::chrono::microseconds(0);
    int compactionNum = 0;

    std::chrono::microseconds firstInsertDuration = std::chrono::microseconds(0);
    std::chrono::microseconds secondInsertDuration = std::chrono::microseconds(0);

    static const uint64_t dimension = 768;

    ThreadPool *pool;
    
public:
    KVStore(const std::string &dir);

    ~KVStore();

    void put(uint64_t key, const std::string &s) override;

    std::string get(uint64_t key) override;

    bool del(uint64_t key) override;

    void reset() override;

    void scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list) override;

    void compaction();

    void delsstable(std::string filename);  // 从缓存中删除filename.sst， 并物理删除
    void addsstable(sstable ss, int level); // 将ss加入缓存

    std::string fetchString(std::string file, int startOffset, uint32_t len);

    std::vector<std::pair<std::uint64_t, std::string>> search_knn(std::string query, int k);

    std::vector<std::pair<std::uint64_t, std::string>> search_knn_hnsw(std::string query, int k);

    void load_embedding_from_disk(std::string path);

    void save_hnsw_index_to_disk(const std::string &hnsw_data_root);
    void load_hnsw_index_from_disk(const std::string &hnsw_data_root);

    // for test
    void output();

    void testEmtable(int max);
};
