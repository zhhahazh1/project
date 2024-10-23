#include <cstdint>
#include <iostream>
#include <class.h>
#include <read.h>
#define KAHYPAR_ATTRIBUTE_FALLTHROUGH __attribute__ ((fallthrough))
#include <vector>
#include <random>
#include <algorithm>

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
  void hash_storage(const size_t hash_num, const size_t Node_Num, std::vector<HashFunc>& hash_functions) {
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
void hash_vectors_calculate(NodeVector& nodes, size_t Node_Num, const size_t hash_num,std::vector<std::vector<HashValue>>& hash_vectors, 
                                                                          std::vector<HashFunc>& hash_functions){
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
std::vector<std::set<Node*>> search_identical_columns(NodeVector& nodes, size_t Node_Num, const size_t hash_num,
                                                    std::vector<std::vector<HashValue>>& hash_vectors) {
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
                    } else if (std::find(cluster.begin(), cluster.end(), nodes[j]) != cluster.end()) {
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
void aggregrate_Nodes(std::vector<std::set<Node*>> clusters,NodeVector& nodes){
    for (auto cluster : clusters) {
    Node* node = new Node(); 
    for (auto Node_identical =cluster.begin();Node_identical!=cluster.end(); Node_identical++) {
      *node = *node + **Node_identical;
      auto del_end = std::remove(nodes.begin(), nodes.end(), *Node_identical);
      nodes.erase(del_end, nodes.end());
    }
    nodes.push_back(node);
  }
}

void buildSparsifiedHypergraph(HyperGraph& HyperGraph,size_t hash_num) {//生成稀疏化后的图
    std::vector<std::vector<HashValue>> hash_vectors;
    std::vector<HashFunc> hash_functions;
    size_t Node_Num = HyperGraph._NumNode;
    NodeVector& nodes = HyperGraph.Node_vector;
    HyperedgeSet& edges=HyperGraph.Edge_vector;
    hash_storage(hash_num, Node_Num, hash_functions);
    std::vector<std::set<Node*>> clusters;

    size_t _Node_Num = nodes.size();
    while ((_Node_Num) > (Node_Num / 2)) {//nodes点少于原本一半后停止聚类
      hash_vectors_calculate(nodes,_Node_Num,hash_num,hash_vectors,hash_functions);
      clusters=search_identical_columns(nodes,_Node_Num,hash_num,hash_vectors);
      bool hasNonEmptyClusters = std::any_of(clusters.begin(), clusters.end(), [](const std::set<Node*>& s){ return !s.empty(); });//clusters不为空时true,可改进
      if(hasNonEmptyClusters){
        aggregrate_Nodes(clusters, nodes);
        _Node_Num = nodes.size();
      }
      else{
        hash_num=hash_num-1;
        hash_storage(hash_num, _Node_Num, hash_functions);
      }
    } 
    HyperGraph.update(nodes,edges);
//去除多余边
}

//解稀疏化
void desparse_Nodes(NodeVector& nodes){//edge的src和没处理,迭代有问题
  NodeVector _nodes = nodes;
  for(auto node =_nodes.begin(); node!=_nodes.end();){
    if(!(*node)->Inclusion_node.empty()){
      for(auto desparse_node:(*node)->Inclusion_node){
        nodes.push_back(desparse_node);
      }
      node=nodes.erase(node);
    }
    else{
      ++node;
    }
}
}

void desparseHypergraph(HyperGraph& HyperGraph){
  NodeVector& nodes = HyperGraph.Node_vector;
}

int main(){
    std::string design_aera = "../exapmle/case01/design.are";
    std::string design_net = "../exapmle/case01/design.net";
    std::string design_info = "../exapmle/case01/design.info";
    std::string design_topo = "../exapmle/case01/design.topo";
    NodeVector nodes = readNodes(design_aera);
    HyperedgeSet Hyperedge =readEdges(design_net, nodes);
    HyperGraph HyperGraph(nodes,Hyperedge);                                                                                                                                                                                     
    buildSparsifiedHypergraph(HyperGraph,3);
    desparse_Nodes(HyperGraph.Node_vector);
    return 0;
    }
