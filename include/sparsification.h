#include <cstdint>
#include <iostream>
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
HyperGraph buildSparsifiedHypergraph(const HyperGraph& hypergraph) {//生成稀疏化后的图
    int n;
//生成sparsifier，计算哈希值（复合哈希值），哈希值全部相同的点放入同一个buckets 
//合并顶点，去除多余边
//cluster数在某个范围后，停止聚类
}



void hash_calculate(const HyperGraph& hypergraph){
    HashFunc hash;
    HashFuncVector<HashFunc> _hash_func_vector;//创建数组存放每个id对应的hash值，seed用什么标识区分？
    //传递hash数，num——vector数
}

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

