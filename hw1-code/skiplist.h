#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdint>
#include <optional>
#include <vector>
#include <random>

namespace skiplist {
using key_type = uint64_t;
using value_type = std::vector<char>;

struct skiplist_node;

class skiplist_type
{
	std::mt19937_64 rng;
	std::geometric_distribution<int> rand_p;
	skiplist_node *top_head;
public:
	explicit skiplist_type(double p = 0.5);
	~skiplist_type();
	void put(key_type key, const value_type &val);
	std::optional<value_type> get(key_type key) const;

	// for hw1 only
	int query_distance(key_type key) const;
};

} // namespace skiplist

#endif // SKIPLIST_H
