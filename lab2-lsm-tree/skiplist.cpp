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

void skiplist::insert(uint64_t key, const std::string &str)
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
    } else {
        // add new key and value
        slnode *node = new slnode(key, str, NORMAL);
        int newLevel = randLevel();
        for (int level = 0; level < newLevel; level++) {
            node->nxt[level] = update[level]->nxt[level];
            update[level]->nxt[level] = node;
        }
        this->bytes += (12 + current->val.length());
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

bool skiplist::del(uint64_t key)
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
    if (current == nullptr || current->key != key) {
        return false;
    } else {
        for (int i = 0; i < curMaxL; i++) {
            if (update[i]->nxt[i] == current) {
                update[i]->nxt[i] = current->nxt[i];
            }
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