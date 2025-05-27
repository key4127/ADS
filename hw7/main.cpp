#include<iostream>
#include<thread>
#include <condition_variable>
#include <mutex>

const int N = 3;

std::mutex mutex;
std::condition_variable cv;
int current = 0;

void print(char c, int id)
{
    for (int i = 0; i < N; i++) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [id] { return current == id; });

        std::cout << c;
        fflush(stdout);
        current = (current + 1) % 3;
        cv.notify_all();
    }
}

int main()
{
    std::thread thread_A(print, 'A', 0);
    std::thread thread_B(print, 'B', 1);
    std::thread thread_C(print, 'C', 2);

    thread_A.join();
    thread_B.join();
    thread_C.join();

    std::cout << std::endl;
}