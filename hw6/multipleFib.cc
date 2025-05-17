#include <thread>
#include <print>
#include <cstdint>
#include <chrono>
#include <cassert>

uint64_t fib_seq(int n)
{
	if (n <= 1)
		return 1;
	return fib_seq(n-1) + fib_seq(n-2);
}

uint64_t fib_parallel(int n, int max_thread)
{
	if (max_thread == 1)
		return fib_seq(n);
	if (n <= 1)
		return 1;
	uint64_t f1;
	int half_thread = (max_thread + 1) / 2;
	std::thread t([&, n, half_thread]() { f1 = fib_parallel(n-1, half_thread); });
	auto f2 = fib_parallel(n-2, max_thread - half_thread);
	t.join();
	return f1 + f2;
}

int main()
{
	for (int i = 1; i <= 40; i++) {
        int thread_num = i;
        std::chrono::nanoseconds totalSeq = std::chrono::nanoseconds(0);
        std::chrono::nanoseconds totalPar = std::chrono::nanoseconds(0);

        for (int j = 0; j < 20; j++) {
            using clock = std::chrono::system_clock;
	        auto t1 = clock::now();
	        auto ans1 = fib_seq(35);
	        auto t2 = clock::now();
	        auto ans2 = fib_parallel(35, thread_num);
	        auto t3 = clock::now();
	        auto time_seq = t2-t1, time_par = t3-t2;
            totalSeq += time_seq;
            totalPar += time_par;
        }

        std::print("{:2} threads, {:.2}\n", i, double(totalSeq.count()) / totalPar.count());
    }

}
