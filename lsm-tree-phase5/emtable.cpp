#include "emtable.h"

emtable::emtable() { }

emtable::emtable(const char *path, ThreadPool *pool) {
    this->loadFile(path, pool);
}

void emtable::reset()
{
    this->dataBlock.clear();
}

void emtable::putFile(const char *path)
{
    if (!utils::dirExists(path)) {
        utils::mkdir(path);
    }
    std::string hPath(path);
    hPath += "embedding";
    path = hPath.data();
    this->pathDir = path;

    FILE *file = fopen(path, "wb");
    fseek(file, 0, SEEK_SET);
    fwrite(&this->dimension, 8, 1, file);

    int size = this->dataBlock.size();
    int n = this->dimension;
    for (int i = 0; i < size; i++) {
        fwrite(&this->dataBlock[i].key, 8, 1, file);
        for (int j = 0; j < n; j++) {
            fwrite(&this->dataBlock[i].vec[j], 4, 1, file);
        }
    }
}

void emtable::loadFile(const char *path, ThreadPool *pool) // to empty datablock
{
    this->reset();

    if (!utils::dirExists(path)) {
        return;
    }

    std::string hPath(path);
    hPath += "embedding";

    FILE *file = fopen(hPath.data(), "rb+");
    fseek(file, 0, SEEK_SET);

    if (fread(&this->dimension, 8, 1, file) != 1) {
        return;
    }

    // to prompt

    int num_threads = std::thread::hardware_concurrency();
    
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    int dataNum = (fileSize - 8) / (8 + this->dimension * 4);
    int dataSize = (8 + this->dimension * 4);
    int chunkSize = dataNum % num_threads ?
                    dataNum / num_threads + 1 :
                    dataNum / num_threads;
    int task_num = (dataNum + chunkSize - 1) / chunkSize;
    std::atomic<int> complete_task_num(0);
    std::vector<std::vector<DataBlock>> originData(num_threads);

    for (int i = 0; i < task_num; i++) {
        pool->enqueue([i, chunkSize, dataSize, dataNum, hPath, &complete_task_num, &originData, this]{
            FILE *tmpFile = fopen(hPath.data(), "rb+");
            fseek(tmpFile, 8 + i * chunkSize * dataSize, SEEK_SET);

            int k = std::min(chunkSize, dataNum - i * chunkSize);
            DataBlock data;

            for (int p = 0; p < k; p++) {
                data.vec.clear();
                fread(&data.key, 8, 1, tmpFile);
                for (int q = 0; q < this->dimension; q++) {
                    float vecI;
                    fread(&vecI, 4, 1, tmpFile);
                    data.vec.push_back(vecI);
                }
                originData[i].push_back(data);
            }

            complete_task_num++;
        });
    }

    while (complete_task_num < task_num) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    }

    std::unordered_set<uint64_t> key;
    for (int i = originData.size() - 1; i >= 0; i--) {
        for (int j = originData[i].size() - 1; j >= 0; j--) {
            if (key.find(originData[i][j].key) == key.end() && !isVecDelete(originData[i][j].vec, pool)) {
                key.insert(originData[i][j].key);
                this->dataBlock.push_back(originData[i][j]);
            } else if (key.find(originData[i][j].key) == key.end()) {
                key.insert(originData[i][j].key);
            }
        }
    }

    /*std::vector<DataBlock> originData;
    DataBlock data;
    //uint64_t tmpKey;
    while (fread(&data.key, 8, 1, file) == 1) {
        data.vec.clear();
        for (int i = 0; i < dimension; i++) {
            float vecI;
            fread(&vecI, 4, 1, file);
            data.vec.push_back(vecI);
        }
        originData.push_back(data);
    }

    std::unordered_set<uint64_t> key;
    for (int i = originData.size() - 1; i >= 0; i--) {
        if (key.find(originData[i].key) == key.end() && (!isVecDelete(originData[i].vec, pool))) {
            key.insert(originData[i].key);
            this->dataBlock.push_back(originData[i]);
        }
    }*/

    printf("dimension is %d, size is %d\n", this->dimension, (int)dataBlock.size());
    if (dataBlock.size() < 5) {
        return;
    }
    for (int i = dataBlock.size() - 1; i >= dataBlock.size() - 5; i--) {
        std::cout << this->dataBlock[i].key << "    ";
        for (int j = 0; j < 5; j++) {
            std::cout << this->dataBlock[i].vec[j] << " ";
        }
        std::cout << std::endl;
    }
}

void emtable::put(uint64_t key, std::vector<float> vec)
{
    DataBlock block;
    block.key = key;
    block.vec = vec;

    dataBlock.push_back(block);
}

void emtable::del(uint64_t key)
{
    std::vector<float> maxVec;
    for (int i = 0; i < dimension; i++) {
        maxVec.push_back(std::numeric_limits<float>::max());
    }

    DataBlock block;
    block.key = key;
    block.vec = maxVec;

    dataBlock.push_back(block);
}

std::vector<float> emtable::get(uint64_t key, ThreadPool *pool)
{
    int num_threads = std::thread::hardware_concurrency();
    int chunk_size = dataBlock.size() % num_threads ?
                     dataBlock.size() / num_threads + 1 :
                     dataBlock.size() / num_threads;
    int getId[num_threads];

    std::mutex getMutex;
    std::atomic<int> complete_task_num(0);

    int cnt = ((int)dataBlock.size() + chunk_size - 1) / chunk_size;
    std::vector<int> id(cnt, -1);

    for (int i = dataBlock.size() - 1; i >= 0; i -= chunk_size) {
        pool->enqueue([i, key, cnt, chunk_size, &id, &complete_task_num, this]{
            int end = std::max(-1, i - chunk_size);
            for (int j = i; j > end; j--) {
                if (dataBlock[j].key == key) {
                    id[cnt - (dataBlock.size() - i - 1) / chunk_size - 1] = j;
                    break;
                }
            }
            complete_task_num++;
        });
    }

    while (complete_task_num < cnt) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    }

    for (int i = cnt - 1; i >= 0; i--) {
        if (id[i] != -1) {
            return dataBlock[id[i]].vec;
        }
    }

    /*for (int i = dataBlock.size() - 1; i >= 0; i--) {
        if (dataBlock[i].key == key) {
            return dataBlock[i].vec;
        }
    }*/

    std::vector<float> maxVec;
    for (int i = 0; i < dimension; i++) {
        maxVec.push_back(std::numeric_limits<float>::max());
    }
    return maxVec;
}

std::vector<DataBlock> emtable::getDataBlock()
{
    return dataBlock;
}

bool emtable::isVecDelete(std::vector<float> vec, ThreadPool *pool)
{
    int num_threads = std::thread::hardware_concurrency();
    int chunk_size = vec.size() % num_threads ?
                     vec.size() / num_threads + 1 :
                     vec.size() / num_threads;

    std::mutex vecMutex;
    std::atomic<int> completed_tasks_count(0);
    std::atomic<bool> isDelete = true;

    for (int i = 0; i < vec.size(); i += chunk_size) {
        pool->enqueue([i, chunk_size, &vec, &isDelete, &completed_tasks_count]{
            int end = std::min((int)vec.size(), i + chunk_size);
            for (int j = i; j < end; j++) {
                if (vec[j] != std::numeric_limits<float>::max()) {
                    isDelete = false;
                    break;
                }
            }
            completed_tasks_count++;
        });
    }

    while (completed_tasks_count < std::min((int)vec.size(), num_threads)) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
    }

    return isDelete;
}