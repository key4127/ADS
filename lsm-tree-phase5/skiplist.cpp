#include "skiplist.h"

double skiplist::my_rand()
{
    return (double)rand() / RAND_MAX;
}

int skiplist::randLevel()
{
    int level = 1;
    while (my_rand() < this->p && level < MAX_LEVEL) {
        level++;
    }
    return level;
}

void skiplist::insert(uint64_t key, const std::string &str, std::vector<float> vec, ThreadPool *pool)
{
    std::vector<slnode*> update(MAX_LEVEL, head);
    slnode *current = head;

    for (int level = curMaxL - 1; level >= 0; level--) {
        while (current->nxt[level] && current->nxt[level]->key < key) {
            current = current->nxt[level];
        }
        update[level] = current;
    }

    current = current->nxt[0];
    if (current && current->key == key) {
        // modify value
        this->bytes += (str.length() - current->val.length());
        current->val = str;

        auto start = std::chrono::high_resolution_clock::now();
        current->vec = vec;
        auto end = std::chrono::high_resolution_clock::now();
        this->duration += std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    } else {
        // add new key and value
        slnode *node = new slnode(key, str, NORMAL);

        auto start = std::chrono::high_resolution_clock::now();
        node->vec = vec;
        auto end = std::chrono::high_resolution_clock::now();
        this->duration += std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        int newLevel = randLevel();
        std::atomic<int> completed_tasks_count(0);
        for (int level = 0; level < newLevel; level++) {
            pool->enqueue([node, update, level, &completed_tasks_count] {
                node->nxt[level] = update[level]->nxt[level];
                update[level]->nxt[level] = node;
                completed_tasks_count++;
            });

            /*node->nxt[level] = update[level]->nxt[level];
            update[level]->nxt[level] = node;*/
        }
        while (completed_tasks_count < newLevel) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }
        this->bytes += (12 + str.length());
    }
}

std::string skiplist::search(uint64_t key)
{
    slnode *current = lowerBound(key);
    if (current && current->key == key) {
        return current->val;
    } else {
        return "";
    }
}

bool skiplist::del(uint64_t key, ThreadPool *pool)
{
    std::vector<slnode*> update(MAX_LEVEL, head);
    slnode *current = head;

    for (int level = curMaxL - 1; level >= 0; level--) {
        while (current->nxt[level] && current->nxt[level]->key < key) {
            current = current->nxt[level];
        }
        update[level] = current;
    }

    current = current->nxt[0];
    if (current == nullptr || (current != nullptr && current->key != key)) {
        return false;
    } else {
        std::atomic<int> completed_tasks_count(0);
        for (int i = 0; i < curMaxL; i++) {
            pool->enqueue([update, current, i, &completed_tasks_count]{
                if (update[i]->nxt[i] == current) {
                    update[i]->nxt[i] = current->nxt[i];
                }
                completed_tasks_count++;
            });

            /*if (update[i]->nxt[i] == current) {
                update[i]->nxt[i] = current->nxt[i];
            }*/
        }
        while (completed_tasks_count < curMaxL) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        }

        while (curMaxL > 1 && head->nxt[curMaxL] == tail) {
            curMaxL--;
        }
        this->bytes -= (12 + current->val.length());
        delete current;
        return true;
    }
}

void skiplist::scan(uint64_t key1, uint64_t key2, std::vector<std::pair<uint64_t, std::string>> &list)
{
    slnode *current = head;

    for (int level = curMaxL; level >= 0; level--) {
        while (current->nxt[level] && current->nxt[level]->key < key1) {
            current = current->nxt[level];
        }
    }

    current = current->nxt[0];

    while (current != tail && current->key <= key2) {
        list.push_back(std::make_pair(current->key, current->val));
        current = current->nxt[0];
    }
}

slnode* skiplist::lowerBound(uint64_t key)
{
    slnode *current = head;
    for (int level = curMaxL - 1; level >= 0; level--) {
        while (current->nxt[level] && current->nxt[level]->key < key) {
            current = current->nxt[level];
        }
    }
    current = current->nxt[0];
    return current;
}

void skiplist::reset()
{
    slnode *current = head->nxt[0];

    // delete all nodes
    while (current != tail) {
        slnode *tmp = current;
        current = current->nxt[0];
        delete tmp;
    }

    // set all levels
    for (int i = 0; i < MAX_LEVEL; i++) {
        head->nxt[i] = tail;
    }

    this->bytes = 0x0;
    this->curMaxL = 1;
}

uint32_t skiplist::getBytes()
{
    return this->bytes;
}

std::vector<std::vector<float>> skiplist::getVec()
{
    std::vector<std::vector<float>> vec;
    slnode *cur = this->getFirst();
    while (cur->type != TAIL) {
        vec.push_back(cur->vec);
        cur = cur->nxt[0];
    }
    return vec;
}

std::vector<uint64_t> skiplist::getKey()
{
    std::vector<uint64_t> vec;
    slnode *cur = this->getFirst();
    while (cur->type != TAIL) {
        vec.push_back(cur->key);
        cur = cur->nxt[0];
    }
    return vec;
}