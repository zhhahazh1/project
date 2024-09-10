#include <iostream>
#include <vector>
#include <algorithm>

// 前向声明
class Node;
class Hyperedge;
class HyperGraph;
class AggNode;

// 节点类
class Node {
public:
    // 带参数构造函数
    Node(const int values[]) {
        std::copy(values, values + 8, area);
    }
    // 默认构造函数
    Node() {
        std::fill(area, area + 8, 0);
    }

    Node operator+(const Node& other) const {
        Node result; // 使用默认构造函数初始化
        for (int i = 0; i < 8; ++i) {
            result.area[i] = this->area[i] + other.area[i];
        }
                    // 合并超边集合
        for (auto* hyperedge : this->hyperedges) {
            result.addHyperedge(hyperedge); // 假设我们有一个方法来添加一个Hyperedge
        }
        for (auto* hyperedge : other.hyperedges) {
            if (std::find(result.hyperedges.begin(), result.hyperedges.end(), hyperedge) == result.hyperedges.end()) {
                result.addHyperedge(hyperedge);
            }
        }
    return result;
    }
    
    void addHyperedge(Hyperedge* hyperedge) {
        hyperedges.push_back(hyperedge);
    }
private:
    std::vector<Node*> nodes;
    std::vector<Hyperedge*> hyperedges;
    int area[8]; // 直接定义为 int 数组
};

// 超边类
class Hyperedge {
public:
    void addNode(Node* node) {
        nodes.push_back(node);
        node->addHyperedge(this);
    }
private:
    std::vector<Node*> nodes;
};


class HyperGraph {
public:
    HyperGraph(std::vector<Node*> Node_vector, std::vector<Hyperedge*> Edge_vector)
        : Node_vector(Node_vector), Edge_vector(Edge_vector), 
          _NumNode(Node_vector.size()), _NumEdge(Edge_vector.size()) 
    {
    }



private:
    std::vector<Node*> Node_vector;
    std::vector<Hyperedge*> Edge_vector;
    size_t _NumNode;
    size_t _NumEdge;
};
