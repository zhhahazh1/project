#include <iostream>
#include <chrono>

// 用于记录函数统计信息的宏
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

// 被统计的函数
FunctionStatistics stats;
void someFunction() {
    // 模拟耗时操作
    START_TIMING();  // 开始计时
    for (volatile int i = 0; i < 1000000; ++i);
    END_TIMING(stats.totalDuration, stats.callCount);  // 结束计时并记录
}

int main() {
    

    // 模拟多次调用
    for (int i = 0; i < 5; ++i) {
        
        someFunction();  // 调用函数
        
    }

    // 打印统计结果
    stats.printStatistics();

    return 0;
}
