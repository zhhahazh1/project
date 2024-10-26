#include <cstdint>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#define KAHYPAR_ATTRIBUTE_FALLTHROUGH __attribute__ ((fallthrough))

template <typename Key>
class MurmurHash {
 public:
  using HashValue = uint64_t;
  explicit MurmurHash(const uint32_t seed = 0) :
    _seed(seed)
  { }
  void reset(const uint32_t seed) {
    _seed = seed;
  }
  inline HashValue operator() (const Key& key) const {
    return hash(reinterpret_cast<const void*>(&key), sizeof(key), _seed);
  }

 private:
  inline uint64_t hash(const void* key, const uint32_t len, const uint32_t seed) const {
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;
    uint64_t h = seed ^ (len * m);
    const uint64_t* data = (const uint64_t*)key;
    const uint64_t* end = data + (len / 8);

    while (data != end) {
      uint64_t k = *data++;
      k *= m;
      k ^= k >> r;
      k *= m;

      h ^= k;
      h *= m;
    }
    const unsigned char* data2 = (const unsigned char*)data;
    switch (len & 7) {
      case 7:
        h ^= uint64_t(data2[6]) << 48;
        KAHYPAR_ATTRIBUTE_FALLTHROUGH;
      case 6:
        h ^= uint64_t(data2[5]) << 40;
        KAHYPAR_ATTRIBUTE_FALLTHROUGH;
      case 5:
        h ^= uint64_t(data2[4]) << 32;
        KAHYPAR_ATTRIBUTE_FALLTHROUGH;
      case 4:
        h ^= uint64_t(data2[3]) << 24;
        KAHYPAR_ATTRIBUTE_FALLTHROUGH;
      case 3:
        h ^= uint64_t(data2[2]) << 16;
        KAHYPAR_ATTRIBUTE_FALLTHROUGH;
      case 2:
        h ^= uint64_t(data2[1]) << 8;
        KAHYPAR_ATTRIBUTE_FALLTHROUGH;
      case 1: h ^= uint64_t(data2[0]);
        h *= m;
    }
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return h;
  }
  uint32_t _seed;
};

using HashFunc = MurmurHash<uint32_t>;
using HashValue = typename HashFunc::HashValue;

//为hash_functions生成hash函数
void hash_storage(const size_t hash_num, std::vector<HashFunc>& hash_functions) {
  hash_functions.reserve(hash_num);//放置hash函数
  uint32_t seed=rand();
  std::default_random_engine eng(seed);
  std::uniform_int_distribution<uint32_t> rnd;
  for (size_t i = 0; i < hash_num; ++i) {
    hash_functions[i].reset(rnd(eng));
  }
}

  //计算顶点邻边的最小hash值
  HashValue minHash(HashFunc hash, std::vector<size_t> edgeIds) {
    HashValue res = std::numeric_limits<HashValue>::max();
    for (auto& it : edgeIds) {
      res = std::min(res, hash(it));
    }
    return res;
  }

//顶点的不同hash函数结合运算
HashValue combinedHash(Node* Node_vector, std::vector<HashFunc> hash_functions) {
   HashValue val = 0;
   std::vector<size_t> edgeIds;
   for (auto& it: Node_vector->hyperedges) {
    edgeIds.push_back(it->id);
   }
   for (uint32_t hash_num = 0; hash_num < hash_functions.size(); ++hash_num) {
    val ^= minHash(hash_functions[hash_num], edgeIds);
    }
    return val;
}

//计算hash_vectors
void hash_vectors_calculate(NodeVector& nodes, size_t Node_Num, const size_t hash_num,std::vector<std::vector<HashValue>>& hash_vectors, std::vector<HashFunc>& hash_functions){
  hash_vectors.resize(hash_num); //为hash_vectors分配适当空间,放置node在不同hash函数下的minhash值
  for (auto& vec : hash_vectors) {
    vec.resize(Node_Num); 
  }
  for (size_t nodes_id=0; nodes_id < Node_Num; ++nodes_id){
    std::vector<size_t> edgeIds;
    Node* vertex=nodes[nodes_id];
    for (auto& it: vertex->hyperedges) {
      edgeIds.push_back(it->id);
    }
    for (size_t i = 0; i < hash_num; ++i) {
      hash_vectors[i][nodes_id] = minHash(hash_functions[i], edgeIds);
    }
  }
}

//寻找hash_vectors中完全相同的数值,相同节点放入同一层cluster
std::vector<std::set<Node*>> search_identical_columns(NodeVector& nodes, size_t Node_Num, const size_t hash_num,std::vector<std::vector<HashValue>>& hash_vectors) {
    std::vector<std::set<Node*>> clusters;
    for (size_t i = 0; i < Node_Num; ++i) {
        for (size_t j = i + 1; j < Node_Num; ++j) {
            bool identical = true;
            for (size_t k = 0; k < hash_num; ++k) {
                if (hash_vectors[k][i] != hash_vectors[k][j]) {
                    identical = false;
                    break;
                }
            }
            if (identical) {
                // 找到相同的列，将它们加入同一个簇
                bool found = false;
                for (auto& cluster : clusters) {
                    if (std::find(cluster.begin(), cluster.end(), nodes[i]) != cluster.end()) {
                        // 如果 i 已经存在于某个簇中，将 j 加入该簇
                        cluster.insert(nodes[j]);
                        found = true;
                        break;
                    } 
                    else if (std::find(cluster.begin(), cluster.end(), nodes[j]) != cluster.end()) {
                        // 如果 j 已经存在于某个簇中，将 i 加入该簇
                        cluster.insert(nodes[i]);
                        found = true;
                        break;
                    }
                }
             if (!found) {
                    // 如果 i 和 j 都不在任何现有簇中，创建一个新的簇
                    clusters.emplace_back(std::set<Node*>{nodes[i], nodes[j]});
                }
        }
    }
  }
  return clusters;
  }

//聚合顶点
void aggregrate_Nodes(std::vector<std::set<Node*>> clusters,NodeVector& nodes,HyperGraph& HyperGraph){
    for (auto cluster : clusters) {
      Node* node = new Node(); 
      for (auto Node_identical =cluster.begin();Node_identical!=cluster.end(); Node_identical++) {
        *node += **Node_identical;
        auto del_end = std::remove(nodes.begin(), nodes.end(), *Node_identical);
        nodes.erase(del_end, nodes.end());
      }
      nodes.push_back(node);
      HyperGraph.Node_vector_all.push_back(node);
    }
}

void buildSparsifiedHypergraph(HyperGraph& HyperGraph,size_t hash_num) {//生成稀疏化后的图
    std::vector<std::vector<HashValue>> hash_vectors;
    std::vector<HashFunc> hash_functions;
    size_t Node_Num = HyperGraph._NumNode;
    NodeVector& nodes = HyperGraph.Node_vector;
    HyperedgeSet& edges=HyperGraph.Edge_vector;
    hash_storage(hash_num, hash_functions);
    std::vector<std::set<Node*>> clusters;

    size_t _Node_Num = Node_Num;
    while ((_Node_Num) > (Node_Num / 2)) {//nodes点少于原本一半后停止聚类
      hash_vectors_calculate(nodes,_Node_Num,hash_num,hash_vectors,hash_functions);
      clusters=search_identical_columns(nodes,_Node_Num,hash_num,hash_vectors);
      bool hasNonEmptyClusters = std::any_of(clusters.begin(), clusters.end(), [](const std::set<Node*>& s){ return !s.empty(); });//clusters不为空时true,可改进
      if(hasNonEmptyClusters){
        aggregrate_Nodes(clusters, nodes,HyperGraph);
        _Node_Num = nodes.size();
      }
      else{
        hash_num=hash_num-1;
        hash_storage(hash_num, hash_functions);
      }
    } 
    for(auto edge:edges){//去除多余边   
      if(edge->nodes.size()==1){
        edges.erase(edge);
      }
    }
    HyperGraph.update(nodes,edges);

}

//解一层稀疏化
HyperGraph desparse_Nodes(HyperGraph& _HyperGraph){
  NodeVector nodes = _HyperGraph.Node_vector;
  HyperedgeSet edges = _HyperGraph.Edge_vector;
  NodeSet nodes_de;
  for(auto node:nodes){
    nodes_de.insert(node);
  }
  for(auto node:nodes){
    if(!node->Inclusion_node.empty()){
      for(auto desparse_node:node->Inclusion_node){//添加inclusion里的node和edge
        nodes_de.insert(desparse_node);
        for(auto edge:desparse_node->hyperedges){
          edge->nodes.insert(desparse_node);
        }
      }
      nodes_de.erase(node);//删除聚合点
      for(auto edge:node->hyperedges){
        edge->nodes.erase(node);
        if(edge->src_node.top()==node){
          edge->src_node.pop();
        }
      }
    }
  }
  for(auto node:nodes_de){//edge解稀疏化
    for(auto edge:node->hyperedges){
      if(edge->nodes.size()!=1){
        edges.insert(edge);
      }
    }
  }
  nodes.clear();
  for(auto node:nodes_de){
    nodes.push_back(node);
  }
  HyperGraph HyperGraph_de(nodes,edges);
  return HyperGraph_de;
}

bool allInclusionNodesEmpty(const NodeVector& nodes) {//判断nodes是否Inclusion_node
    for (const auto* node : nodes) {
        if (!node->Inclusion_node.empty()) {
            return false;
        }
    }
    return true;
}

void desparseHypergraph(HyperGraph& _HyperGraph){//解稀疏化到原来两倍
  NodeVector& nodes = _HyperGraph.Node_vector;
  HyperedgeSet& edges = _HyperGraph.Edge_vector;
  HyperGraph _HyperGraph1(nodes,edges);
  while((!allInclusionNodesEmpty(_HyperGraph1.Node_vector))&(_HyperGraph1.Node_vector.size()<(2*nodes.size()))){
    _HyperGraph1=desparse_Nodes(_HyperGraph1);
  }
  _HyperGraph=_HyperGraph1;
}

//nodes全部解稀疏化
void desparseNodes(NodeVector& nodes){
  while(!allInclusionNodesEmpty(nodes)){
      NodeSet nodes_de;
      for(auto node:nodes){
        nodes_de.insert(node);
      }
      for(auto node:nodes){
        if(!node->Inclusion_node.empty()){
          for(auto desparse_node:node->Inclusion_node){//添加inclusion里的node和edge
            nodes_de.insert(desparse_node);
          }
          nodes_de.erase(node);//删除聚合点
        }
      }
      nodes.clear();
      for(auto node:nodes_de){
        nodes.push_back(node);
      }
  }
}

/*
void minihash(){
    std::srand(static_cast<unsigned int>(std::time(0))); // 设置随机种子

    int a = std::rand() % 10 + 1; // 生成1到10的随机整数
    int b = std::rand() % 10 + 1; // 生成1到10的随机整数
    int p = std::rand() % 10 + 1; // 生成1到10的随机整数

}
int HasyFun(int a,int b,int p){
}
void addnode(Node*node1,Node* node2,HyperGraph &HyperGraph){
    NodeVector &Node_vector = HyperGraph.Node_vector;
    HyperedgeSet &node1_edges=node1->hyperedges,&node2_edges=node2->hyperedges;

    int id1=node1->ID,id2=node2->ID;
    Node* node=*Node_vector[id1]+*Node_vector[id2];
    //修改节点集合
    Node_vector[id1],Node_vector[id2]=nullptr,nullptr;
    Node_vector.push_back(node);
    //修改边的节点集合
    for(auto edge:node1_edges){
        edge->nodes.erase(node1);
        edge->nodes.insert(node);
    }
    for(auto edge:node2_edges){
        edge->nodes.erase(node2);
        edge->nodes.insert(node);
    }
}
void sparsification(HyperGraph &HyperGraph){
    NodeVector Node_vector = HyperGraph.Node_vector;
    Node* node=*Node_vector[0]+*Node_vector[1];
}
*/
