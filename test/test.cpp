#include <cstdint>
#include <iostream>
#include <class.h>
#include <read.h>
#define KAHYPAR_ATTRIBUTE_FALLTHROUGH __attribute__ ((fallthrough))
#include <vector>
#include <random>

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

//为hash_functions和hash_vectors分配适当空间，生成hash函数
  void hash_storage(const size_t hash_num, const size_t Node_Num, std::vector<std::vector<HashValue>>& hash_vectors, std::vector<HashFunc>& hash_functions) {
    hash_functions.reserve(hash_num);//放置hash函数
    uint32_t seed=rand();
    std::default_random_engine eng(seed);
    std::uniform_int_distribution<uint32_t> rnd;
    for (size_t i = 0; i < hash_num; ++i) {
      hash_functions[i].reset(rnd(eng));
    }
    hash_vectors.resize(hash_num); //放置node在不同hash函数下的值
    for (auto& vec : hash_vectors) {
        vec.resize(Node_Num); 
    }
  }
/*
  //计算顶点邻边的最小hash值
  template <typename Iterator>
  HashValue minHash(HashFunc hash, Iterator begin, Iterator end) {
    HashValue res = std::numeric_limits<HashValue>::max();
    for (auto it = begin; it != end; ++it) {
      res = std::min(res, hash(it));
    }
    return res;
  }

//顶点的不同hash函数结合运算
HashValue combinedHash(Node* Node_vector, std::vector<HashFunc> hash_functions) {
   HashValue val = 0;
   for (uint32_t hash_num = 0; hash_num < hash_functions.size(); ++hash_num) {
    auto edges_range = Node_vector->incidentEdges();
    val ^= minHash(hash_functions[hash_num], edges_range.first, edges_range.second);
    }
    return val;
}*/
/*
//计算hash_vectors
std::vector<std::vector<HashValue>> hash_calculate(HyperGraph &HyperGraph, const uint8_t hash_num){
  std::vector<std::vector<HashValue>> hash_vectors;
  std::vector<HashFunc> hash_functions;
  int Node_Num = HyperGraph._NumNode;
  NodeVector& nodes = HyperGraph.Node_vector;
  hash_storage(hash_num, Node_Num, hash_vectors, hash_functions);
  HashValue a=combinedHash(nodes.begin(),hash_functions);
  for (const auto& vertex_id : nodes){
    //(auto vertex_id = nodes.begin(); vertex_id != nodes.end(); ++vertex_id) {
      for (uint32_t i = 0; i < hash_num; ++i) {
        hash_vectors[i] = combinedHash(vertex_id,hash_functions[i]);//二维
      }
    }
  return hash_vectors;
}

HyperGraph buildSparsifiedHypergraph(const HyperGraph& hypergraph) {//生成稀疏化后的图
    int n;
//生成sparsifier，计算哈希值（复合哈希值），哈希值全部相同的点放入同一个buckets 
//合并顶点，去除多余边
//cluster数在某个范围后，停止聚类
}*/

int main(){
    std::string desing_aera = "../exapmle/case01/design.are";
    std::string desing_net = "../exapmle/case01/design.net";
    std::string desing_info = "../exapmle/case01/design.info";
    std::string desing_topo = "../exapmle/case01/design.topo";
    NodeVector nodes = readNodes(desing_aera);
    HyperedgeSet Hyperedge =readEdges(desing_net, nodes);                                                                                                                                                                                     
    HyperGraph HyperGraph(nodes,Hyperedge);
        std::pair<HyperedgeIterator, HyperedgeIterator> incidentEdges() const {
        std::vector<size_t> edgeIds;
        for (auto it = hyperedges.begin(); it != hyperedges.end(); ++it) {
            //edgeIds.push_back((*it)->id);
        }
        return std::make_pair(edgeIds.begin(), edgeIds.end());
    }
}
