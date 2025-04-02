#include "kvstore.h"

#include "skiplist.h"
#include "sstable.h"
#include "utils.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <utility>

#include <fstream>

static const std::string DEL = "~DELETED~";
const uint32_t MAXSIZE       = 2 * 1024 * 1024;

struct poi {
    int sstableId; // vector中第几个sstable
    int pos;       // 该sstable的第几个key-offset
    uint64_t time;
    Index index;
};

struct cmpPoi {
    bool operator()(const poi &a, const poi &b) {
        if (a.index.key == b.index.key)
            return a.time > b.time;
        return a.index.key < b.index.key;
    }
};

KVStore::KVStore(const std::string &dir) :
    KVStoreAPI(dir) // read from sstables
{
    for (totalLevel = 0;; ++totalLevel) {
        std::string path = dir + "/level-" + std::to_string(totalLevel) + "/";
        std::vector<std::string> files;
        if (!utils::dirExists(path)) {
            totalLevel--;
            break; // stop read
        }
        int nums = utils::scanDir(path, files);
        sstablehead cur;
        for (int i = 0; i < nums; ++i) {       // 读每一个文件头
            std::string url = path + files[i]; // url, 每一个文件名
            cur.loadFileHead(url.data());
            sstableIndex[totalLevel].push_back(cur);
            TIME = std::max(TIME, cur.getTime()); // 更新时间戳
        }
    }
}

KVStore::~KVStore()
{
    sstable ss(s);
    if (!ss.getCnt())
        return; // empty sstable
    std::string path = std::string("./data/level-0/");
    if (!utils::dirExists(path)) {
        utils::_mkdir(path.data());
        totalLevel = 0;
    }
    ss.putFile(ss.getFilename().data());
    compaction(); // 从0层开始尝试合并
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &val) {
    uint32_t nxtsize = s->getBytes();
    std::string res  = s->search(key);
    if (!res.length()) { // new add
        nxtsize += 12 + val.length();
    } else
        nxtsize = nxtsize - res.length() + val.length(); // change string
    if (nxtsize + 10240 + 32 <= MAXSIZE)
        s->insert(key, val); // 小于等于（不超过） 2MB
    else {
        sstable ss(s);
        s->reset();
        std::string url  = ss.getFilename();
        std::string path = "./data/level-0";
        if (!utils::dirExists(path)) {
            utils::mkdir(path.data());
            totalLevel = 0;
        }
        addsstable(ss, 0);      // 加入缓存
        ss.putFile(url.data()); // 加入磁盘
        compaction();
        s->insert(key, val);
    }
}

/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key) //
{
    uint64_t time = 0;
    int goalOffset;
    uint32_t goalLen;
    std::string goalUrl;
    std::string res = s->search(key);
    if (res.length()) { // 在memtable中找到, 或者是deleted，说明最近被删除过，
                        // 不用查sstable
        if (res == DEL)
            return "";
        return res;
    }
    for (int level = 0; level <= totalLevel; ++level) {
        for (sstablehead it : sstableIndex[level]) {
            if (key < it.getMinV() || key > it.getMaxV())
                continue;
            uint32_t len;
            int offset = it.searchOffset(key, len);
            if (offset == -1) {
                if (!level)
                    continue;
                else
                    break;
            }
            // sstable ss;
            // ss.loadFile(it.getFilename().data());
            if (it.getTime() > time) { // find the latest head
                time       = it.getTime();
                goalUrl    = it.getFilename();
                goalOffset = offset + 32 + 10240 + 12 * it.getCnt();
                goalLen    = len;
            }
        }
        if (time)
            break; // only a test for found
    }
    if (!goalUrl.length())
        return ""; // not found a sstable
    res = fetchString(goalUrl, goalOffset, goalLen);
    if (res == DEL)
        return "";
    return res;
}
#include<iostream>
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key) { 
    std::string res = get(key);
    if (!res.length())
        return false; // not exist
    put(key, DEL);    // put a del marker
    return true;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset() {
    s->reset(); // 先清空memtable
    std::vector<std::string> files;
    for (int level = 0; level <= totalLevel; ++level) { // 依层清空每一层的sstables
        std::string path = std::string("./data/level-") + std::to_string(level);
        int size         = utils::scanDir(path, files);
        for (int i = 0; i < size; ++i) {
            std::string file = path + "/" + files[i];
            utils::rmfile(file.data());
        }
        utils::rmdir(path.data());
        sstableIndex[level].clear();
    }
    totalLevel = -1;
}

/**
 * Return a list including all the key-value pair between key1 and key2.
 * keys in the list should be in an ascending order.
 * An empty string indicates not found.
 */

struct myPair {
    uint64_t key, time;
    int id, index;
    std::string filename;

    myPair(uint64_t key, uint64_t time, int index, int id,
           std::string file) { // construct function
        this->time     = time;
        this->key      = key;
        this->id       = id;
        this->index    = index;
        this->filename = file;
    }
};

struct cmp {
    bool operator()(myPair &a, myPair &b) {
        if (a.key == b.key)
            return a.time < b.time;
        return a.key > b.key;
    }
};


void KVStore::scan(uint64_t key1, uint64_t key2, std::list<std::pair<uint64_t, std::string>> &list) {
    std::vector<std::pair<uint64_t, std::string>> mem;
    // std::set<myPair> heap; // 维护一个指针最小堆
    std::priority_queue<myPair, std::vector<myPair>, cmp> heap;
    // std::vector<sstable> ssts;
    std::vector<sstablehead> sshs;
    s->scan(key1, key2, mem);   // add in mem
    std::vector<int> head, end; // [head, end)
    int cnt = 0;
    if (mem.size())
        heap.push(myPair(mem[0].first, INF, 0, -1, "qwq"));
    for (int level = 0; level <= totalLevel; ++level) {
        for (sstablehead it : sstableIndex[level]) {
            if (key1 > it.getMaxV() || key2 < it.getMinV())
                continue; // 无交集
            int hIndex = it.lowerBound(key1);
            int tIndex = it.lowerBound(key2);
            if (hIndex < it.getCnt()) { // 此sstable可用
                // sstable ss; // 读sstable
                std::string url = it.getFilename();
                // ss.loadFile(url.data());

                heap.push(myPair(it.getKey(hIndex), it.getTime(), hIndex, cnt++, url));
                head.push_back(hIndex);
                if (it.search(key2) == tIndex)
                    tIndex++; // tIndex为第一个不可的
                end.push_back(tIndex);
                // ssts.push_back(ss); // 加入ss
                sshs.push_back(it);
            }
        }
    }
    uint64_t lastKey = INF; // only choose the latest key
    while (!heap.empty()) { // 维护堆
        myPair cur = heap.top();
        heap.pop();
        if (cur.id >= 0) { // from sst
            if (cur.key != lastKey) {
                lastKey         = cur.key;
                uint32_t start  = sshs[cur.id].getOffset(cur.index - 1);
                uint32_t len    = sshs[cur.id].getOffset(cur.index) - start;
                uint32_t scnt   = sshs[cur.id].getCnt();
                std::string res = fetchString(cur.filename, 10240 + 32 + scnt * 12 + start, len);
                if (res.length() && res != DEL)
                    list.emplace_back(cur.key, res);
            }
            if (cur.index + 1 < end[cur.id]) { // add next one to heap
                heap.push(myPair(sshs[cur.id].getKey(cur.index + 1), cur.time, cur.index + 1, cur.id, cur.filename));
            }
        } else { // from mem
            if (cur.key != lastKey) {
                lastKey         = cur.key;
                std::string res = mem[cur.index].second;
                if (res.length() && res != DEL)
                    list.emplace_back(cur.key, mem[cur.index].second);
            }
            if (cur.index < mem.size() - 1) {
                heap.push(myPair(mem[cur.index + 1].first, cur.time, cur.index + 1, -1, cur.filename));
            }
        }
    }
}


void KVStore::compaction() {
    int curLevel = 0;
    // TODO here
    if (sstableIndex[0].size() <= 2) {
        return;
    }
    //printf("compaction begin\n");

    std::vector<sstable> newTables;
    std::vector<sstablehead> toMergeHeads;
    std::vector<poi> toMergePairs;

    int maxSSNum = 2, toMerge = sstableIndex[0].size();
    uint64_t lowerBound, upperBound;
    int toMergeTableNums;

    bool lastLevel = false;

    for (int level = 0; level <= totalLevel; level++) {

        // num to merge
        if (level) {
            toMerge = sstableIndex[level].size() - maxSSNum;
            if (toMerge <= 0) {
                break;
            }
        }

        newTables.clear();
        toMergeHeads.clear();
        toMergePairs.clear();
        lowerBound = INF, upperBound = 0;
        toMergeTableNums = 0;

        // have to add sth in the next level
        if (!utils::dirExists(std::string("./data/level-") + std::to_string(level + 1))) {
            std::string path = std::string("./data/level-") + std::to_string(level + 1);
            utils::mkdir(path.data());
        }

        // find tables to merge in this level
        std::sort(sstableIndex[level].begin(), sstableIndex[level].end());

        // save terms to be merged
        for (int i = 0; i < toMerge; i++) {
            lowerBound = std::min(lowerBound, sstableIndex[level][i].getMinV());
            upperBound = std::max(upperBound, sstableIndex[level][i].getMaxV());

            poi tmp;
            tmp.sstableId = toMergeTableNums++;
            tmp.time = sstableIndex[level][i].getTime();

            for (int j = 0; j < sstableIndex[level][i].getCnt(); j++) {
                tmp.pos = j;
                tmp.index = sstableIndex[level][i].getIndexById(j);
                toMergePairs.push_back(tmp);
            }

            toMergeHeads.push_back(sstableIndex[level][i]);
        }
        if (level != totalLevel) {
            for (int i = 0; i < sstableIndex[level + 1].size(); i++) {
                poi tmp;
                tmp.sstableId = toMergeTableNums;
                tmp.time = sstableIndex[level + 1][i].getTime();

                if (std::max(sstableIndex[level + 1][i].getMinV(), lowerBound) <= std::min(sstableIndex[level + 1][i].getMaxV(), upperBound)) {
                    for (int j = 0; j < sstableIndex[level + 1][i].getCnt(); j++) {
                        tmp.pos = j;
                        tmp.index = sstableIndex[level + 1][i].getIndexById(j);
                        toMergePairs.push_back(tmp);
                    }

                    toMergeTableNums++;
                    toMergeHeads.push_back(sstableIndex[level + 1][i]);
                }
            }
        }

        // create new SSTables
        std::sort(toMergePairs.begin(), toMergePairs.end(), cmpPoi());
        for (int i = 0; i < toMergePairs.size(); i++) {
            if (i != 0 && toMergePairs[i].index.key == toMergePairs[i - 1].index.key) {
                continue;
            }

            poi p = toMergePairs[i];

            int64_t key = p.index.key;
            uint32_t len;

            std::string goalUrl = toMergeHeads[p.sstableId].getFilename();

            int offset = toMergeHeads[p.sstableId].searchOffset(key, len);
            int goalOffset = offset + 32 + 10240 + 12 * toMergeHeads[p.sstableId].getCnt();

            std::string val = fetchString(goalUrl, goalOffset, len);

            if (lastLevel && val == DEL) {
                continue;
            }

            bool createTable = false;
            if (newTables.empty()) {
                createTable = true;
            } else {
                if (newTables.back().checkSize(val, level + 1, false)) {
                    createTable = true;
                }
            }
            
            if (createTable) {
                newTables.push_back(sstable(s));
            }
 
            newTables.back().insert(key, val);
        }

        // put file
        newTables.back().checkSize("", level + 1, true);

        // save new SSTables
        if (level == totalLevel) {
            totalLevel++;
            lastLevel = true;
        }
        for (auto table : newTables) {
            addsstable(table, level + 1);
        }

        // delete old SSTables
        for (auto head : toMergeHeads) {
            this->delsstable(head.getFilename());
        }

        maxSSNum *= 2;
    }
}

void KVStore::delsstable(std::string filename) {
    for (int level = 0; level <= totalLevel; ++level) {
        int size = sstableIndex[level].size(), flag = 0;
        for (int i = 0; i < size; ++i) {
            if (sstableIndex[level][i].getFilename() == filename) {
                sstableIndex[level].erase(sstableIndex[level].begin() + i);
                flag = 1;
                break;
            }
        }
        if (flag)
            break;
    }
    int flag = utils::rmfile(filename.data());
    if (flag != 0) {
        std::cout << "delete fail!" << std::endl;
        std::cout << strerror(errno) << std::endl;
    }
}

void KVStore::addsstable(sstable ss, int level) {
    sstableIndex[level].push_back(ss.getHead());
}

char strBuf[2097152];

/**
 * @brief Fetches a substring from a file starting at a given offset.
 *
 * This function opens a file in binary read mode, seeks to the specified start offset,
 * reads a specified number of bytes into a buffer, and returns the buffer as a string.
 *
 * @param file The path to the file from which to read the substring.
 * @param startOffset The offset in the file from which to start reading.
 * @param len The number of bytes to read from the file.
 * @return A string containing the read bytes.
 */
std::string KVStore::fetchString(std::string file, int startOffset, uint32_t len) {
    // TODO here
    std::ifstream in(file, std::ios::binary);
    in.seekg(startOffset, std::ios::beg);
    in.read(strBuf, len);
    in.close();
    return std::string(strBuf, in.gcount());
}
