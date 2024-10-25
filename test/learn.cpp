#include <iostream>
#include <vector>
#include <algorithm> // for std::shuffle
#include <random>    // for std::mt19937 and std::uniform_int_distribution
#include <thread>    // for std::thread
#include <mutex>     // for std::mutex
#include <functional>// for std::ref

// 用于输出向量内容的辅助函数
void printVector(const std::vector<int>& vec, std::mutex& coutMutex) {
    std::unique_lock<std::mutex> lock(coutMutex); // 获取锁以保护 std::cout
    for (int num : vec) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

// 每个线程执行的函数
void shuffleAndPrint(std::vector<int> vec, unsigned int seed, std::mutex& coutMutex) {
    std::mt19937 engine(seed); // 使用给定的种子初始化随机数生成器

    // 打乱向量
    std::shuffle(vec.begin(), vec.end(), engine);

    // 打印打乱后的向量
    printVector(vec, coutMutex);
}

int main() {
    const int numThreads = 4; // 线程数量
    const int vecSize = 10;   // 向量大小
    std::vector<int> sharedVec(vecSize); // 共享向量模板
    std::vector<std::thread> threads;
    std::mutex coutMutex; // 用于保护 std::cout 的互斥锁

    // 初始化共享向量模板
    for (int j = 0; j < vecSize; ++j) {
        sharedVec[j] = j; // 填充向量
    }

    // 创建并启动线程
    for (int i = 0; i < numThreads; ++i) {
        // 为每个线程创建一个独立的向量副本
        std::vector<int> vecCopy = sharedVec;
        threads.emplace_back(shuffleAndPrint, std::move(vecCopy), i + 1, std::ref(coutMutex)); // 使用不同的种子
    }

    // 等待所有线程完成
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}