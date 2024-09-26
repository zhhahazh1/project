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
using NodeSet = std::set<Node*>;
using HyperedgeSet = std::set<Hyperedge*>;
using FpgaVector = std::vector<Fpga*>;
using FpgaMap = std::unordered_map<Fpga*, int>;
using GainFpgaMap = std::unordered_map<int,NodeSet>;
// 节点类
class Node {
public:
    NodeSet getneiNode();
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

    // Node operator+(const Node& other) const {
    //     Node result; // 使用默认构造函数初始化
    //     std::set<Hyperedge*> uniqueedge;
    //     for (int i = 0; i < 8; ++i) {
    //         result.area[i] = this->area[i] + other.area[i];
    //     }
    //                 // 合并超边集合
    //     for (auto* hyperedge : this->hyperedges) {
    //         uniqueedge.insert(hyperedge);
    //     }
    //     for (auto* hyperedge : other.hyperedges) {
    //         uniqueedge.insert(hyperedge);
    //     }
    //     for (auto* node : uniqueedge) {
    //         result.hyperedges.insert(node);
    //     }
    // return result;
    // }
    
    void addHyperedge(Hyperedge* hyperedge) {
        hyperedges.insert(hyperedge);
    }

    FpgaMap::iterator maxgain;
    FpgaMap gain;
    HyperedgeSet hyperedges;
    Fpga* fpga;
    FpgaMap neifpga;
    int area[8]; 
    size_t ID;
    bool movenable = true;
};
// 超边类
class Hyperedge {
public:
    FpgaMap getfpgas();
    // NodeSet getSingleOccurrenceFPGANodes();
    Hyperedge(NodeSet nodes,size_t id,int w)
    :nodes(nodes),id(id),weight(w){
        for (auto* node : nodes) {
            node->addHyperedge(this);
        }
    }
    Hyperedge(size_t id):id(id){}
    
    void addNode(Node* node) {
        nodes.insert(node);
        node->addHyperedge(this);
    }
    void setWeight(int w) {
        weight = w;
    }
    Node* src_node;
    NodeSet nodes;
    size_t id;
    int points;
    FpgaMap fpgaCount;
    int weight;
};
//超图类
class HyperGraph {
public:
    HyperGraph(NodeVector Node_vector, HyperedgeSet Edge_vector)
        : Node_vector(Node_vector), Edge_vector(Edge_vector), 
          _NumNode(Node_vector.size()), _NumEdge(Edge_vector.size()) 
    {
    }


    NodeVector Node_vector;
    HyperedgeSet Edge_vector;
    size_t _NumNode;
    size_t _NumEdge;
};
//Fpga类
class Fpga {
public:
    Fpga(const int *area,size_t id) {  
        for (int i = 0; i < 8; ++i) {
            this->area[i] = area[i];
        }
        this->ID=id;
        //this->distance_neifpga = nullptr;
    }
    void initial_Distance(size_t numFpga) {
        this->numFpgas=numFpga;
        this->distance_neifpga.resize(numFpgas, 0);
        //adjList = new std::vector<int>[numFpgas];
    }
    /*void add_node(Node* node){
        this->nodes.insert(node);
        // for(auto edge:node->hyperedges){
        //     this->add_edge(edge);
        // }
    }
    void add_edge(Hyperedge* edge){
        if (std::find(this->edges.begin(), this->edges.end(), edge) == this->edges.end()) {
            this->edges.insert(edge);
    }
    }*/
    NodeVector nodes;
    HyperedgeVector edges;
    int area[8]; 
    size_t ID;
    std::vector<int> distance_neifpga;
    //std::vector<int>* adjList;
    size_t numFpgas;
};
//返回的邻节点不包含自己
NodeSet Node::getneiNode(){
    NodeSet neiNodes;
    for (auto* hyperedge : hyperedges) {
        for (auto* node : hyperedge->nodes) {
            if (node != this) {
                neiNodes.insert(node);
            }
        }
    }
    return neiNodes;
};
//返回的邻居fpga不包含自己所在的fpga
FpgaVector Node::getneifpga(){
    NodeSet neiNodes=this->getneiNode();
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

// NodeSet Hyperedge::getSingleOccurrenceFPGANodes() {
//     // 然后收集只出现了一次的FPGA节点
//     NodeSet singleOccurrenceNodes;
//     for (auto node : this->nodes) {
//         if (this->fpgaCount[node->fpga] == 1) {
//             singleOccurrenceNodes.insert(node);
//         }
//     }
//     return singleOccurrenceNodes;
// }
