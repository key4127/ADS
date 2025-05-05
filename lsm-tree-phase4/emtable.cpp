#include "emtable.h"

emtable::emtable()
{
    this->loadFile(this->pathDir.data());
}

emtable::emtable(const char *path) {
    this->loadFile(path);
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

    /*for (int i = 0; i < dataBlock.size(); i++) {
        printf("the %d key: %lld\n", i, dataBlock[i].key);
        for (int j = 0; j < 5; j++) {
            printf("%lf ", dataBlock[i].vec[j]);
        }
        printf("\n");
    }*/
}

void emtable::loadFile(const char *path) // to empty datablock
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
    std::vector<DataBlock> originData;
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
        if (key.find(originData[i].key) == key.end() && (!isVecDelete(originData[i].vec))) {
            key.insert(originData[i].key);
            this->dataBlock.push_back(originData[i]);
        }
    }

    /*for (int i = 0; i < dataBlock.size(); i++) {
        printf("the %d key: %lld\n", i, dataBlock[i].key);
        for (int j = 0; j < 5; j++) {
            printf("%lf ", dataBlock[i].vec[j]);
        }
        printf("\n");
    }*/
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

std::vector<float> emtable::get(uint64_t key)
{
    for (int i = dataBlock.size() - 1; i >= 0; i--) {
        if (dataBlock[i].key == key) {
            return dataBlock[i].vec;
        }
    }

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

bool emtable::isVecDelete(std::vector<float> vec)
{
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] != std::numeric_limits<float>::max()) {
            return false;
        }
    }

    return true;
}