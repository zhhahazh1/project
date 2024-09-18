#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>

// 前向声明
class Node;
class Hyperedge;
class HyperGraph;
class Fpga;
#include <unordered_map>
using NodeVector = std::vector<Node*>;
using HyperedgeVector = std::vector<Hyperedge*>;
using FpgaVector = std::vector<Fpga*>;
using FpgaMap = std::unordered_map<Fpga*, int>;
using nodemove = std::pair<Node*,Fpga*>;
// 节点类
class Node {
public:
    NodeVector getneiNode();
    FpgaVector getneifpga();
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
    int gain=0;
    NodeVector nodes;
    HyperedgeVector hyperedges;
    Fpga* fpga;
    FpgaMap neifpga;
    int area[8]; 
    size_t ID;
};
// 超边类
class Hyperedge {
public:
    FpgaMap getfpgas();
    NodeVector getSingleOccurrenceFPGANodes();
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
    FpgaMap fpgaCount;
    int weight;
};
//超图类
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
//Fpga类
class Fpga {
public:
    Fpga(const int *id) {  
        for (int i = 0; i < 2; ++i) {
            this->ID[i] = id[i];
        }
    }
    void add_node(Node* node){
        this->nodes.push_back(node);
        for(auto edge:node->hyperedges){
            this->add_edge(edge);
        }
    }
    void add_edge(Hyperedge* edge){
        if (std::find(this->edges.begin(), this->edges.end(), edge) == this->edges.end()) {
            this->edges.push_back(edge);
    }
    }
    int ID[2];
    NodeVector nodes;
    HyperedgeVector edges;
    int area[8]; 
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

FpgaVector Node::getneifpga(){
    NodeVector neiNodes=this->getneiNode();
    FpgaVector neifpgas;
    std::set<Fpga*> uniqueNeiFpga;
    for (auto node:neiNodes){
        if (node->fpga != this->fpga) {
            uniqueNeiFpga.insert(node->fpga);
        }
    }
    for (auto node:uniqueNeiFpga){
        neifpgas.push_back(node);
    }
    return neifpgas;
};

NodeVector Hyperedge::getSingleOccurrenceFPGANodes() {
    // 然后收集只出现了一次的FPGA节点
    NodeVector singleOccurrenceNodes;
    for (auto node : this->nodes) {
        if (this->fpgaCount[node->fpga] == 1) {
            singleOccurrenceNodes.push_back(node);
        }
    }
    return singleOccurrenceNodes;
}
