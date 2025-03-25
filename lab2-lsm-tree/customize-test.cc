#include "kvstore.h"

#include <iostream>
#include <ctime>

const uint64_t SMALL = 1536; // 2MB
const uint64_t MEDIUM = 1024 * 4;  // 8MB
const uint64_t LARGE = 1024 * 8; // 32MB

void test(uint64_t max)
{
    KVStore store("./data");
    store.reset();

    clock_t put_beg, put_end;
    clock_t get_beg, get_end;
    clock_t del_beg, del_end;

    put_beg = clock();
    for (int i = 0; i < max; i++) {
        store.put(i, std::string(i + 1, 's'));
    }
    put_end = clock();

    get_beg = clock();
    for (int i = 0; i < max; i++) {
        store.get(i);
    }
    get_end = clock();

    del_beg = clock();
    for (int i = 0; i < max; i++) {
        store.del(i);
    }
    del_end = clock();

    // total time cost (s)
    double put_time = (double)(put_end - put_beg) / CLOCKS_PER_SEC;
    double get_time = (double)(get_end - get_beg) / CLOCKS_PER_SEC;
    double del_time = (double)(del_end - del_beg) / CLOCKS_PER_SEC;

    // throughput
    double put_throughput = 1 / put_time * max;
    double get_throughput = 1 / get_time * max;
    double del_throughput = 1 / del_time * max;

    // average latency (ms)
    double put_latency = put_time * 1000 / max;
    double get_latency = get_time * 1000 / max;
    double del_latency = del_time * 1000 / max;

    printf("throughput: \n");
    printf("put: %.8lf, get: %.8lf, del: %.8lf\n\n", put_throughput, get_throughput, del_throughput);

    printf("latency: \n");
    printf("put: %.8lf, get: %.8lf, del: %.8lf\n\n", put_latency, get_latency, del_latency);
}

int main()
{
    test(LARGE);
}