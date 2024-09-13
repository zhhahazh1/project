#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

// 前向声明
class Node;
class Hyperedge;
class HyperGraph;
using NodeVector = std::vector<Node*>;
using HyperedgeVector = std::vector<Hyperedge*>;
// 节点类
class Node {
public:
    // 带参数构造函数
    Node(const int *area,size_t id) {  
        for (int i = 0; i < 8; ++i) {
            this->area[i] = area[i];
        }
        this->ID=id;
    }
    // 默认构造函数
    Node() {
        std::fill(area, area + 8, 0);
    }

    Node operator+(const Node& other) const {
        Node result; // 使用默认构造函数初始化
        std::set<Hyperedge*> uniqueedge;
        for (int i = 0; i < 8; ++i) {
            result.area[i] = this->area[i] + other.area[i];
        }
                    // 合并超边集合
        for (auto* hyperedge : this->hyperedges) {
            uniqueedge.insert(hyperedge);
        }
        for (auto* hyperedge : other.hyperedges) {
            uniqueedge.insert(hyperedge);
        }
        for (auto* node : uniqueedge) {
            result.hyperedges.push_back(node);
        }
    return result;
    }
    
    void addHyperedge(Hyperedge* hyperedge) {
        hyperedges.push_back(hyperedge);
    }
    NodeVector getneiNode();

    NodeVector nodes;
    HyperedgeVector hyperedges;
    int area[8]; // 直接定义为 int 数组
    size_t ID;
};

// 超边类
class Hyperedge {
public:
    Hyperedge(NodeVector nodes,size_t id,int w)
    :nodes(nodes),id(id),weight(w){
        for (auto* node : nodes) {
            node->addHyperedge(this);
        }
    }
    Hyperedge(size_t id):id(id){}
    
    void addNode(Node* node) {
        nodes.push_back(node);
        node->addHyperedge(this);
    }
    void setWeight(int w) {
        weight = w;
    }

    NodeVector nodes;
    size_t id;
    int weight;
};


class HyperGraph {
public:
    HyperGraph(NodeVector Node_vector, HyperedgeVector Edge_vector)
        : Node_vector(Node_vector), Edge_vector(Edge_vector), 
          _NumNode(Node_vector.size()), _NumEdge(Edge_vector.size()) 
    {
    }


    NodeVector Node_vector;
    HyperedgeVector Edge_vector;
    size_t _NumNode;
    size_t _NumEdge;
};

NodeVector Node::getneiNode(){
    NodeVector neiNodes;
    std::set<Node*> uniqueNeiNodes; // Use a set to avoid duplicates

    for (auto* hyperedge : hyperedges) {
        for (auto* node : hyperedge->nodes) {
            if (node != this) {
                uniqueNeiNodes.insert(node);
            }
        }
    }

    // Convert the set to a vector
    for (auto* node : uniqueNeiNodes) {
        neiNodes.push_back(node);
    }

    return neiNodes;
};