#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <stack>

// 前向声明
class Node;
class Hyperedge;
class HyperGraph;
class Fpga;

using NodeVector = std::vector<Node*>;
using NodeSet = std::set<Node*>;
using HyperedgeSet = std::set<Hyperedge*>;
using FpgaVector = std::vector<Fpga*>;
using FpgaSet = std::set<Fpga*>;
using FpgaMap = std::unordered_map<Fpga*, int>;
using NodeMove = std::pair<Node*, Fpga*>;
using GainFpgaMap = std::map<int, std::set<NodeMove>>;
//using HyperedgeIterator = HyperedgeSet::const_iterator;
using HyperedgeIterator = typename std::vector<size_t>::const_iterator;

class Node;
class Hyperedge;
class HyperGraph;
class Fpga;
int dis_fpgas(Fpga* fpga1,Fpga* fpga2);
// 面积类
class Area {
    public:
        int values[8];

        // 构造函数
        Area() {
            std::fill(std::begin(values), std::end(values), 0);
        }
        Area(const int arr[8]) {
            std::copy(&arr[0], &arr[0] + 8, values);
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
        Area operator*(double scale) const {
            Area result;
            for (int i = 0; i < 8; ++i) {
                result.values[i] = this->values[i] * scale;
            }
            return result;
        }

        // 运算符重载：大小比较
        bool operator<=(const Area& other) const {
            for (int i = 0; i < 8; ++i) {
                if (this->values[i] > other.values[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator>(const Area& other) const {
            for (int i = 0; i < 8; ++i) {
                if (this->values[i] > other.values[i]) {
                    return true;
            }
        }
    return false;
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
    
    // 带参数构造函数
    Node(const int *area,size_t id) {  
        this->area = Area(area);
        this->ID=id;
    }
    Node() : area() {
        // 默认构造函数
    }
    Node(const Node& other)
        :area(other.area), ID(other.ID){
    }
    Node operator+(Node& other);
    Node& operator+=(Node& other);
    void update_hyperedges_less();
    
    void addHyperedge(Hyperedge* hyperedge) {
        hyperedges.insert(hyperedge);
        hyperedges_less.insert(hyperedge);
    }
    std::string getid(){
        std::string id=std::to_string(ID)+"(";
        if(Inclusion_node.size()==0){
            return std::to_string(ID);
        }
        else{
            for(auto innode:Inclusion_node){
                id+=innode->getid()+",";
            }
        }
        return id+")";
    }

    NodeSet getneiNode();
    FpgaSet getneifpga();
    void inifpgae(Fpga* fpga);
    //HyperedgeSet getneiedge();
    NodeSet Inclusion_node;
    FpgaMap gain;
    std::unordered_map<Fpga*, std::unordered_map<Hyperedge*,int>> gain_edge;
    HyperedgeSet hyperedges;
    HyperedgeSet hyperedges_less;
    Fpga* fpga=nullptr; 
    Area area;
    size_t ID=0;
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
    Hyperedge(const Hyperedge& other)
        :id(other.id), weight(other.weight){
    }
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
    //返回超边中距离源节点超过maxdistance的节点
    NodeSet geterror_nodes(int maxdistance){
        Fpga* src_fpga=this->src_node.top()->fpga;
        NodeSet error_nodes;
        for (auto node:nodes){
            int distance=dis_fpgas(src_fpga, node->fpga);
            if (distance>maxdistance){
                error_nodes.insert(node);
            }
        }
        return error_nodes;
    }
    void setWeight(int w) {
        weight = w;
    }
    void updateFpgaCount() {
        fpgaCount.clear();
        for (auto node : nodes) {
            fpgaCount[node->fpga] += 1;
        }
    }
    std::stack<Node*> src_node;
    NodeSet nodes;
    size_t id;
    int points=0;
    FpgaMap fpgaCount;
    int weight;
};
//超图类
class HyperGraph {
public:
    HyperGraph(NodeVector Node_vector, HyperedgeSet Edge_vector)
        : Node_vector(Node_vector), Edge_vector(Edge_vector), Node_vector_all(Node_vector), Edge_vector_all(Edge_vector),
          _NumNode(Node_vector.size()), _NumEdge(Edge_vector.size()) 
    {
    }
    HyperGraph() : _NumNode(0), _NumEdge(0) {
        // 默认构造函数
    }
    void clear(){
        for(auto node:Node_vector){
            delete node;
        }
        for(auto edge:Edge_vector){
            delete edge;
        }
    }
    void remove(){
        for(auto node:Node_vector){
            node->movenable=true;
        }
    }
     // 深拷贝构造函数
    HyperGraph(const HyperGraph& other) {
        _NumNode = other._NumNode;
        _NumEdge = other._NumEdge;

        // 创建映射用于存储旧对象到新对象的映射[旧，新]
        std::unordered_map<Node*, Node*> nodeMap;
        std::unordered_map<Hyperedge*, Hyperedge*> edgeMap;
        // 创建映射用于存储新对象到旧对象的映射
        std::unordered_map<Node*, Node*> renodeMap;
        std::unordered_map<Hyperedge*, Hyperedge*> reedgeMap;

        // 先复制所有节点
        for (const auto& node : other.Node_vector_all) {
            Node* newNode = new Node(*node);  // 复制节点数据
            this->Node_vector_all.push_back(newNode);
            nodeMap[node] = newNode;
            renodeMap[newNode] = node;
        }

        // 再复制所有边
        for (const auto& edge : other.Edge_vector_all) {
            Hyperedge* newEdge = new Hyperedge(*edge);  // 复制边数据
            this->Edge_vector_all.insert(newEdge);
            edgeMap[edge] = newEdge;
            reedgeMap[newEdge] = edge;
        }
        // 更新节点与节点之间的引用
        for (const auto& node : this->Node_vector_all) {
            for (const auto& neiNode : renodeMap[node]->Inclusion_node) {
                node->Inclusion_node.insert(nodeMap[neiNode]);
            }
        }

        // 更新节点与边之间的引用
        for (const auto& node : this->Node_vector_all) {
            for (const auto& edge : renodeMap[node]->hyperedges) {
                node->hyperedges.insert(edgeMap[edge]);  // 添加新的边引用
            }
            for (const auto& edge : renodeMap[node]->hyperedges_less) {
                node->hyperedges_less.insert(edgeMap[edge]);  // 添加新的边引用
            }
        }

        for (const auto& edge : this->Edge_vector_all) {
            std::stack<Node*> tempStack = reedgeMap[edge]->src_node;//复制一份操作
            std::stack<Node*> mid;
            while (!tempStack.empty()) {
                auto node = tempStack.top();
                tempStack.pop();
                mid.push(nodeMap[node]);
            }
            while (!mid.empty()) {
                edge->src_node.push(mid.top());
                mid.pop();
            }
            for (const auto& node : reedgeMap[edge]->nodes) {
                edge->nodes.insert(nodeMap[node]);  // 添加新的节点引用
            }
        }

        //更新node_vector和edge_vector
        for (const auto& node : other.Node_vector) {
            this->Node_vector.push_back(nodeMap[node]);
        }
        for (const auto& edge : other.Edge_vector) {
            this->Edge_vector.insert(edgeMap[edge]);
        }
    }

    void update(NodeVector newNodes, HyperedgeSet newEdges) {
        Node_vector = newNodes;
        Edge_vector = newEdges;
        _NumNode=Node_vector.size();
        _NumEdge=Edge_vector.size();
    }
    void update_edge_fpgacont(){
        for (auto edge : Edge_vector) {
            edge->updateFpgaCount();
        }
    }

    NodeVector Node_vector;
    HyperedgeSet Edge_vector;
    NodeVector Node_vector_all;
    HyperedgeSet Edge_vector_all;
    size_t _NumNode;
    size_t _NumEdge;
};
//Fpga类
class Fpga {
public:
    Fpga(const int *area,size_t id) {  
        for (int i = 0; i < 8; ++i) {
            this->area.values[i] = area[i];
        }
        this->ID=id;
        //this->distance_neifpga = nullptr;
    }
        // 深拷贝构造函数
    Fpga(const Fpga& other)
        : area(other.area), ID(other.ID),
          distance_neifpga(other.distance_neifpga),  // 复制 vector
          numFpgas(other.numFpgas), maxcoppoints(other.maxcoppoints)
          {
    }

    void initial_Distance(size_t numFpga) {
        this->numFpgas=numFpga;
        this->distance_neifpga.resize(numFpgas, 0);
        //adjList = new std::vector<int>[numFpgas];
    }
    void desparse_Node(Node* node){
        this->nodes.erase(node);
        for(auto node:node->Inclusion_node){
            node->fpga=this;
            this->nodes.insert(node);
        }
    }
    void add_node(Node* node){
        if(node->fpga!=nullptr){
            throw std::invalid_argument("node'fpga is not nullptr");
        }
        this->nodes.insert(node);
        //修改已使用面积
        this->usearea+=node->area;
        node->inifpgae(this);
        for(auto edge:node->hyperedges_less){
            edge->fpgaCount[this]+=1;
            if(edge->fpgaCount.size()>1&&edge->fpgaCount[this]==1){
                this->nowcoppoints+=edge->weight;
            }
            //此后这条边被切割了，把原本的fpga的coppoints加上
            if(edge->fpgaCount.size()==2&&edge->fpgaCount[this]==1){
                for(auto fpga:edge->fpgaCount){
                    if(fpga.first!=this){
                        fpga.first->nowcoppoints+=edge->weight;
                    }
                }
            }
        }
    }
    void erase_node(Node* node){
        this->nodes.erase(node);
        //修改已使用面积
        this->usearea-=node->area;
        node->fpga=nullptr;
        for(auto edge:node->hyperedges_less){
            edge->fpgaCount[this]-=1;
            if(edge->fpgaCount[this]==0){
                this->nowcoppoints-=edge->weight;
                edge->fpgaCount.erase(this);
                //此后这条边不被切割的话，把原本的fpga的coppoints减去
                if(edge->fpgaCount.size()==1){
                    for(auto fpga:edge->fpgaCount){
                        fpga.first->nowcoppoints-=edge->weight;
                    }
                }
            } 
        }
    }

    
//打印子节点id
    void print(){
        std::cout << "FPGA" << ID << " "<<this->nodes.size()<<" ";
        std::cout << nowcoppoints << " ";
        // int count = 0;
        // int max_nodes_to_print = 10;
        // for (auto* node : this->nodes) {
        //     if (count >= max_nodes_to_print) {
        //         std::cout << "...";
        //         break;
        //     }
        //     std::cout << node->getid() << ", ";
        //     ++count;
        // }
        std::cout << std::endl;
    }
    void print_nowcoppoints(){
        std::cout << "FPGA" << ID << " area: " << this->area << std::endl;
    }
    Area usearea;
    NodeSet nodes;
    HyperedgeSet edges;
    //int area[8]; 
    Area area;
    size_t ID;
    std::vector<int> distance_neifpga;
    //std::vector<int>* adjList;
    size_t numFpgas;
    int maxcoppoints;
    int nowcoppoints=0;
    int checkcoppoints=0;
    bool areaconstrain;
    bool copconstrain;
};
//返回的邻节点不包含自己
NodeSet Node::getneiNode(){
    NodeSet neiNodes;
    HyperedgeSet neiedges=this->hyperedges_less;
    for (auto* hyperedge : neiedges) {
        for (auto* node : hyperedge->nodes) {
            if (node != this) {
                neiNodes.insert(node);
            }
        }
    }
    return neiNodes;
};
// HyperedgeSet Node::getneiedge(){
//         HyperedgeSet neiedges;
//         for (auto edge :this->hyperedges_less){
//             if(edge->nodes.size()!=1){
//                 neiedges.insert(edge);
//             }
//         }
//         return hyperedges;
// };
//返回的邻居fpga,不包含自己所在的fpga
FpgaSet Node::getneifpga(){
    FpgaSet neifpgas;
    for (auto hyperedge : hyperedges) {
        FpgaMap fpgas=hyperedge->fpgaCount;
        for (auto fpga:fpgas){
            if (fpga.first != this->fpga) {
                neifpgas.insert(fpga.first);
            }
        }
    }
    return neifpgas;
};
void Node::inifpgae(Fpga* fpga){
        this->fpga=fpga;
}

int dis_fpgas(Fpga* fpga1,Fpga* fpga2){
        return fpga1->distance_neifpga[fpga2->ID];
}

Node Node::operator+(Node& other) {
    Node result(*this); // 使用当前对象初始化

    //节点面积相加
    result.area = this->area + other.area;

    //两个子节点的超边均连向result节点，使用set避免重复
    std::set<Hyperedge*> uniqueedge(this->hyperedges.begin(), this->hyperedges.end());
    uniqueedge.insert(other.hyperedges.begin(), other.hyperedges.end());
    
    result.hyperedges = std::move(uniqueedge);  // 将uniqueedge内容转移到result的超边集合中
    for (Hyperedge* hyperedge : result.hyperedges) { // 遍历result的所有超边，并从每个超边的节点集合中删除this和other,加入result
            hyperedge->nodes.insert(this);
            hyperedge->nodes.erase(&other);
            if(hyperedge->src_node.top()==&other){
            hyperedge->src_node.push(this);
            }
    }

    // 当前节点和other节点都要插入到result节点的nodes集合中
    result.Inclusion_node.insert(this);
    result.Inclusion_node.insert(&other);
    return result;
}
Node& Node::operator+=(Node& other) {
    //节点面积相加
    this->area += other.area;

    //两个子节点的超边均连向result节点，使用set避免重复
    std::set<Hyperedge*> uniqueedge(this->hyperedges.begin(), this->hyperedges.end());
    uniqueedge.insert(other.hyperedges.begin(), other.hyperedges.end());
    this->hyperedges = std::move(uniqueedge);  // 将uniqueedge内容转移到result的超边集合中

    for (Hyperedge* hyperedge : this->hyperedges) { // 遍历result的所有超边，并从每个超边的节点集合中删除this和other,加入result
            hyperedge->nodes.insert(this);
            hyperedge->nodes.erase(&other);
            if(hyperedge->src_node.top()==&other){
                hyperedge->src_node.push(this);
            }
    }
    this->update_hyperedges_less();

    // 当前节点和other节点都要插入到result节点的nodes集合中
    this->Inclusion_node.insert(&other);
    return *this;
}

void Node::update_hyperedges_less() {
    //清空hyperedges_less
    this->hyperedges_less.clear();
    //重新添加
    for (auto edge : this->hyperedges) {
        if (edge->nodes.size() != 1) {
            this->hyperedges_less.insert(edge);
        }
    }
}
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
