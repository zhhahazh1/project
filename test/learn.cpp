#include <iostream>
#include <map>
#include <vector>
#include <string>

bool operator<(const std::vector<int>& lhs, const std::vector<int>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

int main() {
    std::map<std::vector<int>, std::vector<std::string>> myMap;

    std::vector<int> key1 = {1, 2, 3};
    std::vector<int> key2 = {4, 5, 6};
    std::vector<int> key3 = {3, 2, 1};

    myMap[key1].push_back("First result for key1");
    myMap[key1].push_back("Second result for key1");
    myMap[key2].push_back("First result for key2");
    myMap[key3].push_back("First result for key3");
    for (const auto& pair : myMap) {
        std::cout << "{ ";
        for (int num : pair.first) {
            std::cout << num << " ";
        }
        std::cout << "} : ";
        for (const auto& result : pair.second) {
            std::cout << result << "; ";
        }
        std::cout << std::endl;
    }

    return 0;
}
