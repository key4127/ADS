#include "sstablehead.h"

#include <cstring>
#include <iostream>

void sstablehead::loadFileHead(const char *path, ThreadPool *pool) { // 只读取文件头
    FILE *file = fopen(path, "rb+");               // 注意格式为二进制
    filename   = path;
    int len = std::strlen(path), c = 0;
    std::string suf;
    for (int i = 0; i < len; ++i) {
        if (c == 2)
            suf += path[i];
        if (path[i] == '-') {
            c++;
        }
        if (path[i] == '.')
            c = 0;
    }
    if (suf.size())
        nameSuffix = std::stoi(suf);
    else
        nameSuffix = 0;
    fseek(file, 0, SEEK_SET);
    reset();

    fread(&time, 8, 1, file);
    fread(&cnt, 8, 1, file);
    fread(&minV, 8, 1, file);
    fread(&maxV, 8, 1, file);
    for (int i = 0; i < M * 8; i += 8) { // bloom
        unsigned char cur = 0x0;
        fread(&cur, 1, 1, file);
        for (int j = 0; j < 8; ++j) {
            if ((cur >> j) & 1)
                filter.setBit(i + j);
        }
    }
    Index temp;
    bytes = 10240 + 32 + 12 * cnt;

    /*for (int i = 0; i < cnt; ++i) { // index
        fread(&temp.key, 8, 1, file);
        fread(&temp.offset, 4, 1, file);
        index.push_back(temp);
    }*/

    int num_threads = std::thread::hardware_concurrency();

    fseek(file, 0, SEEK_END);
    int dataSize = 12;
    int chunkSize = cnt % num_threads ?
                    cnt / num_threads + 1 :
                    cnt / num_threads;
    int task_num = (cnt + chunkSize - 1) / chunkSize;

    std::atomic<int> complete_task_num(0);
    std::vector<std::vector<Index>> tmpIndex(num_threads);

    for (int i = 0; i < task_num; i++) {
        pool->enqueue([i, chunkSize, dataSize, path, &tmpIndex, &complete_task_num, this]{
            FILE *tmpFile = fopen(path, "rb+");
            fseek(tmpFile, 32 + M + i * chunkSize * dataSize, SEEK_SET);

            int k = std::min(chunkSize, (int)cnt - i * chunkSize);
            Index temp;

            for (int j = 0; j < k; j++) {
                fread(&temp.key, 8, 1, tmpFile);
                fread(&temp.offset, 4, 1, tmpFile);
                tmpIndex[i].push_back(temp);
            }

            fclose(tmpFile);
            complete_task_num++;
        });
    }

    while (complete_task_num < task_num) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    }

    for (int i = 0; i < task_num; i++) {
        for (int j = 0; j < tmpIndex[i].size(); j++) {
            index.push_back(tmpIndex[i][j]);
        }
    }

    //bytes += temp.offset;
    bytes += tmpIndex[task_num - 1][tmpIndex[task_num - 1].size() - 1].offset;
    fflush(file);
    fclose(file);
}

void sstablehead::reset() {
    filter.reset();
    index.clear();
}

int sstablehead::search(uint64_t key) {
    int res = filter.search(key);
    if (!res)
        return -1; // bloom 说没有 确实没有
    auto it = std::lower_bound(index.begin(), index.end(), Index(key, 0));
    if (it == index.end())
        return -1; // 没找到
    if ((*it).key == key)
        return it - index.begin(); // 在这一块二分找到了，返回第几个字符串
    return -1;
}

int sstablehead::searchOffset(uint64_t key, uint32_t &len) {
    int res = filter.search(key);
    if (!res)
        return -1; // bloom 说没有 确实没有
    auto it = std::lower_bound(index.begin(), index.end(), Index(key, 0));
    if (it == index.end())
        return -1; // 没找到
    if ((*it).key == key) {
        if (it == index.begin()) {
            len = (*it).offset;
            return 0;
        } else {
            len = (*it).offset - (*(it - 1)).offset;
            return (*(it - 1)).offset;
        }
    }
    return -1;
}

int sstablehead::lowerBound(uint64_t key) {
    auto it = std::lower_bound(index.begin(), index.end(), Index(key, 0));
    return it - index.begin(); // found
}
