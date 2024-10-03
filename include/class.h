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
// 面积类
class Area {
    public:
        int values[8];

        // 构造函数
        Area() {
            std::fill(std::begin(values), std::end(values), 0);
        }

        // 运算符重载：加法
        Area operator+(const Area& other) const {
            Area result;
            for (int i = 0; i < 8; ++i) {
                result.values[i] = this->values[i] + other.values[i];
            }
            return result;
        }
        Area& operator+=(const Area& other) {
            for (int i = 0; i < 8; ++i) {
                this->values[i] += other.values[i];
            }
            return *this;
        }
        Area& operator-=(const Area& other) {
            for (int i = 0; i < 8; ++i) {
                this->values[i] -= other.values[i];
            }
            return *this;
        }
        Area operator-(const Area& other) const {
            Area result;
            for (int i = 0; i < 8; ++i) {
                result.values[i] = this->values[i] - other.values[i];
            }
            return result;
        }

        // 运算符重载：大小比较
        bool operator<(const Area& other) const {
            for (int i = 0; i < 8; ++i) {
                if (this->values[i] >= other.values[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator>(const Area& other) const {
            for (int i = 0; i < 8; ++i) {
                if (this->values[i] <= other.values[i]) {
                    return false;
                }
            }
            return true;
        }

        // 输出运算符重载
        friend std::ostream& operator<<(std::ostream& os, const Area& area) {
            os << "[";
            for (int i = 0; i < 8; ++i) {
                os << area.values[i];
                if (i < 7) os << ", ";
            }
            os << "]";
            return os;
        }
};
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

    Node* operator+(Node& other) {
        Node* result = new Node; // 使用默认构造函数初始化

        //节点面积相加
        result->area = this->area + other.area;

        //两个子节点的超边均连向result节点，使用set避免重复
        std::set<Hyperedge*> uniqueedge(this->hyperedges.begin(), this->hyperedges.end());
        uniqueedge.insert(other.hyperedges.begin(), other.hyperedges.end());
        
        result->hyperedges = std::move(uniqueedge);  // 将uniqueedge内容转移到result的超边集合中

        // 当前节点和other节点都要插入到result节点的nodes集合中
        result->nodes.insert(this);
        result->nodes.insert(&other);
        return result;
    }
    
    void addHyperedge(Hyperedge* hyperedge) {
        hyperedges.insert(hyperedge);
    }
    NodeSet nodes;
    FpgaMap::iterator maxgain;
    FpgaMap gain;
    HyperedgeSet hyperedges;
    Fpga* fpga;
    FpgaMap neifpga;
    //int area[8]; 
    Area area;
    size_t ID;
    bool movenable = true;
};
// 超边类
class Hyperedge {
public:
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
    FpgaMap getFpgaMap(){
        return fpgaCount;
    }
    // int getmaxdistance(){
    //     Fpga* src_fpga=this->src_node->fpga;
    //     int max_distance=0;
    //     for (auto fpga:fpgaCount){
    //         if (fpga.first!=src_fpga){
    //             distance=dis_fpgas(src_fpga,fpga.first);
    //             max_distance=distance>max_distance?distance:max_distance;
    //         }
    //     }
    //     return max_distance;
    // };
    NodeSet geterror_nodes(int maxdistance){
        Fpga* src_fpga=this->src_node->fpga;
        for (auto node:nodes){
            int distance=dis_fpgas(src_fpga,node->fpga);
            if (distance>maxdistance){
                this->error_nodes.insert(node);
            }
        }
        return this->error_nodes;
    }
    void setWeight(int w) {
        weight = w;
    }
    Node* src_node;
    NodeSet nodes;
    NodeSet error_nodes;
    size_t id;
    int points=0;
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
    void add_node(Node* node){
        this->nodes.push_back(node);
        //修改已使用面积
        this->usearea+=node->area;
        // for(auto edge:node->hyperedges){
        //     this->add_edge(edge);
        // }
    }
    void add_edge(Hyperedge* edge){
        if (std::find(this->edges.begin(), this->edges.end(), edge) == this->edges.end()) {
            this->edges.insert(edge);
        }
    }

    //int usearea[8]={0,0,0,0,0,0,0,0};
    Area usearea;
    NodeVector nodes;
    HyperedgeSet edges;
    //int area[8]; 
    Area area;
    size_t ID;
    std::vector<int> distance_neifpga;
    //std::vector<int>* adjList;
    size_t numFpgas;
    int maxcoppoints;
    int nowcoppoints;
    bool areaconstrain;
    bool copconstrain;
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
