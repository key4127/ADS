#ifndef __cpp_lib_print
# define CANNOT_USE_PRINT
#endif

#include <limits>
#include <random>
#include <sstream>
#include <utility>
#include <vector>
#include <cassert>
#include <cstdint>
#ifdef CANNOT_USE_PRINT
# include <cstdio>
#else
# include <print>
#endif
#include <cstdlib>

#include "skiplist.h"

std::mt19937_64 rng;

std::vector<std::pair<skiplist::key_type, skiplist::value_type>>
gen_input(int element_count, uint64_t seed)
{
	constexpr int VALUE_LEN_MIN = 1;
	constexpr int VALUE_LEN_MEAN = 20;
	rng.seed(seed);
	std::uniform_int_distribution<skiplist::key_type> rand_key(
		std::numeric_limits<skiplist::key_type>::min(),
		std::numeric_limits<skiplist::key_type>::max());
	auto gen_key = [&]() {
		return rand_key(rng);
	};
	std::geometric_distribution<> rand_len(1.0 / (VALUE_LEN_MEAN - VALUE_LEN_MIN));
	std::uniform_int_distribution<> rand_byte(
		std::numeric_limits<char>::min(), std::numeric_limits<char>::max());
	auto gen_value = [&]() {
		int len = rand_len(rng) + VALUE_LEN_MIN;
		skiplist::value_type value;
		value.reserve(len);
		for (int i = 0; i < len; ++i)
			value.push_back(rand_byte(rng));
		return value;
	};
	std::vector<std::pair<skiplist::key_type, skiplist::value_type>> kvpairs;
	kvpairs.reserve(element_count);
	for (int i = 0; i < element_count; ++i)
		kvpairs.emplace_back(gen_key(), gen_value());
	return kvpairs;
}

constexpr int QUERY_TIMES = 10000;
constexpr long SEED = 15;

void mainFunc(int element_count, double p) {
	auto input = gen_input(element_count, SEED);

	// construct kvstore with input data
	auto kvstore = skiplist::skiplist_type(p);
	for (auto &[key, value] : input){
		// print the generated data
		// printf("%d %s \n",key,value.c_str());
		kvstore.put(key, std::move(value));
	}
	
	// random query
	int64_t query_distance_sum = 0;
	std::uniform_int_distribution<> rand_idx(0, element_count - 1);
	for (int i = 0; i < QUERY_TIMES; ++i)
	{
		int x = rand_idx(rng);
		auto key = input[x].first;
		query_distance_sum += kvstore.query_distance(key);
	}

#ifdef CANNOT_USE_PRINT
	printf("element count: %d, p: %lf, %lf\n", element_count, p, double(query_distance_sum) / QUERY_TIMES);
#else
	std::print("{}", double(query_distance_sum) / QUERY_TIMES);
#endif
}

// argv[1] = element count
// argv[2] = seed
// argv[3] = possibility p
int main(int argc, const char *argv[])
{
	/*
	assert(argc == 4);
	int element_count = std::atoi(argv[1]);
	auto seed = std::atoll(argv[2]);
	double p = std::atof(argv[3]);
	

	// generate input data
	auto input = gen_input(element_count, seed);

	// construct kvstore with input data
	auto kvstore = skiplist::skiplist_type(p);
	for (auto &[key, value] : input){
		// print the generated data
		// printf("%d %s \n",key,value.c_str());
		kvstore.put(key, std::move(value));
	}
	
	// random query
	int64_t query_distance_sum = 0;
	std::uniform_int_distribution<> rand_idx(0, element_count - 1);
	for (int i = 0; i < QUERY_TIMES; ++i)
	{
		int x = rand_idx(rng);
		auto key = input[x].first;
		query_distance_sum += kvstore.query_distance(key);
	}

#ifdef CANNOT_USE_PRINT
	printf("%lf", double(query_distance_sum) / QUERY_TIMES);
#else
	std::print("{}", double(query_distance_sum) / QUERY_TIMES);
#endif
	*/

	int element[5] = {50, 100, 200, 500, 1000};
	double p[5] = {0.5, 1 / exp(1), 0.25, 0.125};
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 4; j++) {
			mainFunc(element[i], p[j]);
		}
		printf("\n");
	}
}
