#include "HNSW.h"

HNSW::HNSW()
{
    this->dimension = 768;
    this->nodeNum = 0;

    m_M = 8;
    m_L = 6;
    efConstruction = 35;
    M_max = 18;

    this->totalLevel = m_L + 1;

    for (int i = 0; i <= m_L; i++) {
        this->head.push_back(nullptr);
    }
}

void HNSW::reset()
{
    head.clear();
    nodes.clear();
    deleted.clear();

    this->dimension = 768;
    this->nodeNum = 0;

    m_M = 8;
    m_L = 6;
    efConstruction = 35;
    M_max = 18;

    this->totalLevel = m_L + 1;

    for (int i = 0; i <= m_L; i++) {
        this->head.push_back(nullptr);
    }
}

int HNSW::randLevel()
{
    return rand() % (m_L + 1);
}

std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> HNSW::searchLayer(
    std::vector<float> vec, HWnode* entry, int layer, int num)
{
    std::unordered_set<uint64_t> visit;
    std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> search; // the best 
    std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNworst> result; // the worst

    int n = vec.size();
    float entrySim = common_embd_similarity_cos(vec.data(), entry->vec.data(), n);
    visit.insert(entry->key);
    search.push(std::make_pair(entry, entrySim));
    if (!isDeleted(entry->id, entry->vec)) { // not delete
        result.push(std::make_pair(entry, entrySim));
    }

    HWnode* current; // the nearest in search

    while (!search.empty()) {
        if (result.size() != 0 && search.top().second < result.top().second) {
            break;
        }

        current = search.top().first;
        search.pop();

        for (auto e : current->next[layer]) {

            if (visit.find(nodes[e]->key) == visit.end()) { // not visit
                visit.insert(nodes[e]->key);
                float currentSim = common_embd_similarity_cos(vec.data(), nodes[e]->vec.data(), n);

                if (result.size() < num || currentSim > result.top().second) {
                    search.push(std::make_pair(nodes[e], currentSim));
                    if (!isDeleted(nodes[e]->id, nodes[e]->vec)) {
                        result.push(std::make_pair(nodes[e], currentSim));
                    }
                }
            }
        }

        if (result.size() > num) {
            int currentSize = result.size();
            for (int i = currentSize; i > num; i--) {
                result.pop();
            }
        }
    }

    std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> ans;
    while (!result.empty()) {
        std::pair<HWnode*, float> tmp = result.top();
        if (!isDeleted(tmp.first->id, tmp.first->vec)) {
            ans.push(tmp);
        }
        result.pop();
    }

    return ans;
}

void HNSW::insert(uint64_t key, std::vector<float> vec)
{
    bool isChange = false;
    int originNode;
    for (int i = nodes.size() - 1; i >= 0; i--) {
        if (nodes[i]->key == key && this->deleted.find(i) == deleted.end()) {
            isChange = true;
            originNode = i;
            break;
        }
    }
    if (isChange) {
        this->del(key, nodes[originNode]->vec);
    }

    int n = vec.size();

    HWnode *node = new HWnode(key, vec);
    node->id = nodeNum;
    this->nodeNum++;
    uint32_t level = this->randLevel();
    node->level = level;
    nodes.push_back(node);

    float nearestSim = std::numeric_limits<float>::max();
    HWnode* entry;
    if (totalLevel <= m_L) {
        entry = head[totalLevel];
    }

    for (int l = totalLevel; l > level; l--) {
        if (l > m_L) {
            break;
        }

        std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> result = 
            this->searchLayer(vec, entry, l, 1);
        
        if (result.size() != 0) {
            entry = result.top().first;
        }  else {
            entry = head[l];
        }
    }

    bool firstLevel = true;

    for (int l = level; l >= 0; l--) {

        if (level > totalLevel || totalLevel > m_L) {
            head[l] = node;
            continue;
        }

        if (firstLevel) { // the first level that is not empty
            entry = head[l];
            firstLevel = false;
        }

        std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> result = 
            this->searchLayer(vec, entry, l, efConstruction);

        entry = result.top().first;

        for (int i = 0; i < m_M; i++) {
            if (result.empty()) {
                break;
            }

            std::pair<HWnode*, float> tmp = result.top();

            if (tmp.first->next[l].size() >= M_max) {
                auto furthest = tmp.first->next[l].begin();
                float maxSim = std::numeric_limits<float>::max();
                float currentSim;

                for (auto j = tmp.first->next[l].begin(); j != tmp.first->next[l].end(); j++) {
                    currentSim = common_embd_similarity_cos(vec.data(), (nodes[*j])->vec.data(), n);
                    if (currentSim < maxSim) {
                        furthest = j;
                        maxSim = currentSim;
                    }
                }

                HWnode *toDeleteNei = (nodes[*furthest]);
                tmp.first->next[l].erase(furthest);

                auto origin = toDeleteNei->next[l].begin();
                for (; origin != toDeleteNei->next[l].end(); origin++) {
                    if (nodes[*origin]->key == tmp.first->key) {
                        break;
                    }
                }

                if (origin == toDeleteNei->next[l].end()) {
                    // error
                    exit(1);
                }

                toDeleteNei->next[l].erase(origin);
            }

            node->next[l].push_back(tmp.first->id);
            tmp.first->next[l].push_back(node->id);

            result.pop();
        }
    }

    if (totalLevel <= m_L) {
        totalLevel = std::max(level, totalLevel);
    } else {
        totalLevel = level;
    }
}

std::vector<uint64_t> HNSW::query(std::vector<float> vec, int k)
{
    HWnode* entry = head[totalLevel];

    for (int l = totalLevel; l > 0; l--) {
        std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> result = 
            this->searchLayer(vec, entry, l, 1);

        if (result.size() != 0){
            entry = result.top().first;
        } else {
            entry = head[l];
        }
    }

    std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> result = 
            this->searchLayer(vec, entry, 0, efConstruction);

    std::vector<uint64_t> ans;

    for (int i = 0; i < k; i++) {
        if (result.empty()) {
            break;
        }
        ans.push_back(result.top().first->key);
        result.pop();
    }

    return ans;
}

void HNSW::del(uint64_t key, std::vector<float> vec) // to prompt
{
    const HWnode *toDelete = nullptr;
    for (int i = nodes.size() - 1; i >= 0; i--) {
        if (nodes[i]->key == key) {
            if (this->deleted.find(i) == deleted.end()) {
                toDelete = nodes[i];
            }
            break;
        }
    }

    if (toDelete != nullptr) {
        deleted[toDelete->id] = vec;
    }
}

bool HNSW::isDeleted(uint32_t id, std::vector<float> vec)
{
    if (deleted.find(id) != deleted.end()) {
        return true;
    }

    return false;
}

void HNSW::putFile(const std::string &pathDir)
{
    const char *path = pathDir.data();

    if (!utils::dirExists(path)) {
        utils::mkdir(path);
    }

    FILE *file;

    const std::string header = pathDir + "global_header.bin";
    const char *header_path = header.data();
    file = fopen(header_path, "wb");
    fseek(file, 0, SEEK_SET);
    fwrite(&m_M, 4, 1, file);
    fwrite(&M_max, 4, 1, file);
    fwrite(&efConstruction, 4, 1, file);
    fwrite(&m_L, 4, 1, file);
    fwrite(&totalLevel, 4, 1, file);
    fwrite(&nodeNum, 4, 1, file);
    fwrite(&dimension, 4, 1, file);
    fclose(file);

    const std::string deleted_string = pathDir + "deleted_nodes.bin";
    const char *deleted_path = deleted_string.data();
    file = fopen(deleted_path, "wb");
    fseek(file, 0, SEEK_SET);
    int i = 0;
    for (const auto pair : this->deleted) {
        fwrite(&pair.first, 4, 1, file);

        std::vector<float> vec = pair.second;
        for (const auto v : vec) {
            fwrite(&v, 4, 1, file);
        }
        i++;
    }
    fclose(file);

    const std::string node_string = pathDir + "nodes/";
    const char *node_path = node_string.data();
    if (!utils::dirExists(node_path)) {
        utils::mkdir(node_path);
    }

    for (int i = 0; i < nodeNum; i++) {
        this->putNode(node_string, i);
    }
}

void HNSW::loadFile(const std::string &pathDir, const std::vector<DataBlock> &data)
{
    this->reset();

    const char *path = pathDir.data();
    if (!utils::dirExists(path)) {
        return;
    }

    FILE *file;

    const std::string header = pathDir + "global_header.bin";
    const char *header_path = header.data();
    file = fopen(header_path, "rb+");
    fseek(file, 0, SEEK_SET);
    fread(&m_M, 4, 1, file);
    fread(&M_max, 4, 1, file);
    fread(&efConstruction, 4, 1, file);
    fread(&m_L, 4, 1, file);
    fread(&totalLevel, 4, 1, file);
    fread(&nodeNum, 4, 1, file);
    fread(&dimension, 4, 1, file);
    fclose(file);

    const std::string deleted = pathDir + "deleted_nodes.bin";
    const char *deleted_path = deleted.data();
    uint32_t id;
    std::vector<float> vec;
    file = fopen(deleted_path, "rb+");
    fseek(file, 0, SEEK_SET);
    while (fread(&id, 4, 1, file) == 1) {
        vec.clear();
        for (int i = 0; i < dimension; i++) {
            float v;
            fread(&v, 4, 1, file);
            vec.push_back(v);
        }
        this->deleted.insert({id, vec});
        printf("delete %d, vec is %f\n", id, vec[0]);
    }
    fclose(file);

    const std::string node_string = pathDir + "nodes/";
    const char *node_path = node_string.data();

    if (!utils::dirExists(node_path)) {
        // node num = 0
        return;
    } else {
        for (int i = 0; i < nodeNum; i++) {
            this->loadNode(node_string, data, i);
        }

        for (int i = 0; i < nodeNum; i++) {
            if (head[nodes[i]->level] == nullptr) {
                head[nodes[i]->level] = nodes[i];
            }
        }
    }
}

void HNSW::putNode(const std::string &pathDir, int k)
{
    std::string nodeDir = pathDir + std::to_string(k) + "/";
    
    const char *node_path = nodeDir.data();
    if (!utils::dirExists(node_path)) {
        utils::mkdir(node_path);
    }

    FILE *file;

    std::string header = nodeDir + "header.bin";
    const char *header_path = header.data();
    file = fopen(header_path, "wb");
    fseek(file, 0, SEEK_SET);
    fwrite(&nodes[k]->level, 4, 1, file);
    fwrite(&nodes[k]->key, 8, 1, file); // ?
    fclose(file);

    std::string edgeDir = nodeDir + "edges/";

    const char *edges_path = edgeDir.data();
    if (!utils::dirExists(edges_path)) {
        utils::mkdir(edges_path);
    }

    for (int i = 0; i <= nodes[k]->level; i++) {
        const std::string edge = edgeDir + std::to_string(i) + ".bin";
        const char *edge_path = edge.data();
        file = fopen(edge_path, "wb");
        fseek(file, 0, SEEK_SET);

        uint32_t neiNum = nodes[k]->next[i].size();
        fwrite(&neiNum, 4, 1, file);

        for (int j = 0; j < neiNum; j++) {
            fwrite(&nodes[k]->next[i][j], 4, 1, file);
        }

        fclose(file);
    }
}

void HNSW::loadNode(const std::string &pathDir, const std::vector<DataBlock> &data, int k)
{
    std::string nodeDir = pathDir + std::to_string(k) + "/";
    FILE *file;

    HWnode *node = new HWnode();
    node->id = k;

    std::string header = nodeDir + "header.bin";
    const char *header_path = header.data();
    file = fopen(header_path, "rb+");
    fseek(file, 0, SEEK_SET);
    fread(&node->level, 4, 1, file);
    fread(&node->key, 8, 1, file);
    fclose(file);

    if (deleted.find(k) != deleted.end()) { // to change
        node->vec = deleted[k];
    } else {
        for (int i = data.size() - 1; i >= 0; i--) {
            if (data[i].key == node->key) {
                node->vec = data[i].vec;
                break;
            }
        }
    }

    node->next.resize(m_L + 1);
    std::string edgeDir = nodeDir + "edges/";
    for (int i = 0; i <= node->level; i++) {
        const std::string edge = edgeDir + std::to_string(i) + ".bin";
        const char *edge_path = edge.data();
        file = fopen(edge_path, "rb+");
        fseek(file, 0, SEEK_SET);

        uint32_t neiNum;
        fread(&neiNum, 4, 1, file);

        for (int j = 0; j < neiNum; j++) {
            uint32_t neiId;
            fread(&neiId, 4, 1, file);
            node->next[i].push_back(neiId);
        }

        fclose(file);
    }

    nodes.push_back(node);
}