#include <vector>
#include <iostream>
#include <random>

int main() {
    // 固定的初始种子
    unsigned int initial_seed = 12345;
    
    // 使用初始种子初始化随机数生成器
    std::mt19937 generator(initial_seed);

    // 创建一个向量来存储生成的种子
    std::vector<unsigned int> seeds;
    int num_seeds = 10; // 生成 10 个种子

    // 生成多个种子
    for (int i = 0; i < num_seeds; ++i) {
        unsigned int new_seed = generator();
        seeds.push_back(new_seed);
        std::cout << "Seed " << i + 1 << ": " << new_seed << std::endl;
    }

    // 创建一个范围在 [0, 10] 的随机数分布
    std::uniform_int_distribution<int> dist(0, 10);

    // 使用每个生成的种子初始化引擎，并生成 0 到 10 的随机数
    for (int i = 0; i < 10; i++) {
        // 使用每个种子初始化随机数引擎
        std::default_random_engine engine(seeds[i]);
        
        // 生成 0 到 10 的随机数
        int random_value = dist(engine);
        
        // 输出种子和对应的随机数
        std::cout << "Seed " << seeds[i] << " generated value: " << random_value << std::endl;
    }

    return 0;
}
// int main() {
//     // 初始种子
//     unsigned int initial_seed = 12345;
    
//     // 使用初始种子初始化随机数生成器
//     std::mt19937 generator(initial_seed);

//     // 创建一个向量来存储生成的种子
//     std::vector<unsigned int> seeds;
//     int num_seeds = 10; // 生成 10 个种子

//     // 生成多个种子
//     for (int i = 0; i < num_seeds; ++i) {
//         unsigned int new_seed = generator();
//         seeds.push_back(new_seed);
//         std::cout << "Seed " << i + 1 << ": " << new_seed << std::endl;
//     }

//     return 0;
// }