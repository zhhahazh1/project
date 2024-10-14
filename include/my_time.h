#include <iostream>
#include <chrono>
#define START_TIMING() auto start = std::chrono::high_resolution_clock::now();
#define END_TIMING(totalDuration, callCount) \
    auto end = std::chrono::high_resolution_clock::now(); \
    totalDuration += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
    callCount++;

// 统计类
class FunctionStatistics {
public:
    // 打印统计结果
    void printStatistics() const {
        std::cout << "Total time: " << totalDuration << " microseconds\n";
        std::cout << "Total calls: " << callCount << "\n";
    }

    // 记录总时长和总次数
    long long totalDuration = 0;
    int callCount = 0;
};
FunctionStatistics stats;


