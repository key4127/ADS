#include "HNSW.h"

HNSW::HNSW()
{
    this->totalLevel = -1;
    
    m_M = 8;
    m_L = 6;
    efConstruction = 35;
    M_max = 18;

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
    result.push(std::make_pair(entry, entrySim));

    HWnode* current; // the nearest in search

    while (!search.empty()) {
        if (search.top().second < result.top().second) {
            break;
        }

        current = search.top().first;
        search.pop();

        for (auto e : current->next[layer]) {

            if (visit.find(e->key) == visit.end()) { // not visit
                visit.insert(e->key);
                float currentSim = common_embd_similarity_cos(vec.data(), e->vec.data(), n);

                if (result.size() < num || currentSim > result.top().second) {
                    search.push(std::make_pair(e, currentSim));
                    result.push(std::make_pair(e, currentSim));
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
        ans.push(tmp);
        result.pop();
    }

    return ans;
}

void HNSW::insert(uint64_t key, std::vector<float> vec)
{
    int n = vec.size();

    HWnode *node = new HWnode(key, vec);
    int level = this->randLevel();
    float nearestSim = std::numeric_limits<float>::max();
    HWnode* entry;
    if (totalLevel != -1) {
        entry = head[totalLevel];
    }

    for (int l = totalLevel; l > level; l--) {

        std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> result = 
            this->searchLayer(vec, entry, l, 1);
        
        entry = result.top().first;
    }

    bool firstLevel = true;

    for (int l = level; l >= 0; l--) {

        if (level > totalLevel) {
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
                    currentSim = common_embd_similarity_cos(vec.data(), (*j)->vec.data(), n);
                    if (currentSim < maxSim) {
                        furthest = j;
                        maxSim = currentSim;
                    }
                }

                HWnode *toDeleteNei = (*furthest);
                tmp.first->next[l].erase(furthest);

                auto origin = toDeleteNei->next[l].begin();
                for (; origin != toDeleteNei->next[l].end(); origin++) {
                    if ((*origin)->key == tmp.first->key) {
                        break;
                    }
                }

                if (origin == toDeleteNei->next[l].end()) {
                    // error
                    exit(1);
                }

                toDeleteNei->next[l].erase(origin);
            }

            node->next[l].push_back(tmp.first);
            tmp.first->next[l].push_back(node);

            result.pop();
        }
    }

    totalLevel = std::max(level, totalLevel);
}

std::vector<uint64_t> HNSW::query(std::vector<float> vec, int k)
{
    HWnode* entry = head[totalLevel];

    for (int l = totalLevel; l > 0; l--) {

        std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> result = 
            this->searchLayer(vec, entry, l, 1);

        entry = result.top().first;
    }

    std::priority_queue<std::pair<HWnode*, float>, std::vector<std::pair<HWnode*, float>>, HNbest> result = 
            this->searchLayer(vec, entry, 0, efConstruction);

    std::vector<uint64_t> ans;

    for (int i = 0; i < k; i++) {
        ans.push_back(result.top().first->key);
        result.pop();
    }

    return ans;
}