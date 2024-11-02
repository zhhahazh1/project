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
  hash_functions.clear();
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
void hash_vectors_calculate(NodeSet& nodes, size_t Node_Num, const size_t hash_num,std::vector<std::vector<std::pair<Node*, HashValue>>>& hash_vectors, std::vector<HashFunc>& hash_functions){
  hash_vectors.clear();
  hash_vectors.resize(hash_num); //为hash_vectors分配适当空间,放置node在不同hash函数下的minhash值


  for (auto vertex:nodes){
    std::vector<size_t> edgeIds;
    for (auto& it: vertex->hyperedges_less) {
      edgeIds.push_back(it->id);
    }
    for (size_t i = 0; i < hash_num; ++i) {
      HashValue min_hash = minHash(hash_functions[i], edgeIds);
      hash_vectors[i].push_back(std::make_pair(vertex, min_hash));
    }
  }
}

bool operator<(const std::vector<HashValue>& lhs, const std::vector<HashValue>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
//寻找hash_vectors中完全相同的数值,相同节点放入同一层cluster
std::vector<std::set<Node*>> search_identical_columns(NodeSet& nodes, size_t Node_Num, const size_t hash_num,std::vector<std::vector<std::pair<Node*, HashValue>>>& hash_vectors) {
    std::vector<std::set<Node*>> clusters;
    std::map<std::vector<HashValue>, std::set<Node*>> myMap;
    std::vector<HashValue> key;
    for (size_t i = 0; i < Node_Num; ++i) {
      key.clear();
      for(size_t j=0;j<hash_num;j++){
        auto it=hash_vectors[j][i];
        key.push_back(hash_vectors[j][i].second);
      }
      myMap[key].insert(hash_vectors[0][i].first);
    }
    for (auto& it : myMap) {
      if(it.second.size()!=1){
        clusters.push_back(it.second);
      }
    }
    return clusters;
}

//聚合顶点
void aggregrate_Nodes(std::vector<std::set<Node*>> clusters,NodeSet& nodes,HyperGraph& HyperGraph,int area){
    int i=0;
    for (auto cluster : clusters) {
      auto it = cluster.begin();
      NodeVector nodedel;
      while (it != cluster.end()) {
        if((*it)->area.values[0] > area){
          it++;
        }
        else{
          int id=HyperGraph.Node_vector_all.size();
          Node* node = new Node(id);
          // 使用一个独立的迭代器进行内层循环
          auto inner_it = it;
          while (inner_it != cluster.end()) {
              node->addNode(*inner_it);  // 累加值
              i++;
              std::cout <<  i <<  std::endl;
              nodes.erase(*inner_it);  // 从Node_all中删除
              if (node->area.values[0] > area) {
                ++inner_it;
                  break;  // 如果面积超过 500，停止
              }
              ++inner_it;  // 移动到下一个元素
          }
          HyperGraph.Node_vector_all.push_back(node);  // 加入All_node
          nodes.insert(node);  // 加入Node_all
          node->update_hyperedges_less();
          it = inner_it;  // 更新外层迭代器
        }
      }
      
    }
}



void buildSparsifiedHypergraph(HyperGraph& HyperGraph,size_t hash_num,int n,int level,int area) {//生成稀疏化后的图
    std::vector<std::vector<std::pair<Node*, HashValue>>> hash_vectors;
    std::vector<HashFunc> hash_functions;
    size_t Node_Num = HyperGraph._NumNode;
    NodeSet& nodes = HyperGraph.Node_vector;
    HyperedgeSet& edges=HyperGraph.Edge_vector;
    
    std::vector<std::set<Node*>> clusters;

    size_t _Node_Num = Node_Num;
    while ((_Node_Num) > (Node_Num / n)) {//nodes点少于原本一半后停止聚类
      hash_storage(hash_num, hash_functions);
      hash_vectors_calculate(nodes,_Node_Num,hash_num,hash_vectors,hash_functions);
      clusters=search_identical_columns(nodes,_Node_Num,hash_num,hash_vectors);
      bool hasNonEmptyClusters = std::any_of(clusters.begin(), clusters.end(), [](const std::set<Node*>& s){ return !s.empty(); });//clusters不为空时true,可改进
      if(hasNonEmptyClusters){
        int _Node_Num2 = nodes.size();
        aggregrate_Nodes(clusters, nodes,HyperGraph,area);
        _Node_Num = nodes.size();
        hash_num=_Node_Num2-_Node_Num>level?hash_num:hash_num-1;
        if(hash_num<1){
          if(_Node_Num-Node_Num / n>(_Node_Num2-_Node_Num)*10){
            break;
          }
          hash_num=1;
        }
      }
      else{
        hash_num=hash_num-1;
        hash_num=hash_num<1?1:hash_num;
      }
    } 
    std::queue<Hyperedge*> q;
    for(auto edge:edges){//去除多余边   
      if(edge->nodes.size()==1){
        q.push(edge);
      }
    }
    while (!q.empty()) {
      auto edge = q.front();
      q.pop();
      edges.erase(edge);
    }
    HyperGraph.update(nodes,edges);

}

void buildSparsifiedHypergraph2(HyperGraph& HyperGraph,size_t hash_num,int level,int area) {//生成稀疏化后的图
    std::vector<std::vector<std::pair<Node*, HashValue>>> hash_vectors;
    std::vector<HashFunc> hash_functions;
    size_t Node_Num = HyperGraph._NumNode;
    NodeSet& nodes = HyperGraph.Node_vector;
    HyperedgeSet& edges=HyperGraph.Edge_vector;
    
    std::vector<std::set<Node*>> clusters;

    size_t _Node_Num = Node_Num;
    while ((_Node_Num) > (Node_Num / 8)) {//nodes点少于原本一半后停止聚类
      hash_storage(hash_num, hash_functions);
      hash_vectors_calculate(nodes,_Node_Num,hash_num,hash_vectors,hash_functions);
      clusters=search_identical_columns(nodes,_Node_Num,hash_num,hash_vectors);
      bool hasNonEmptyClusters = std::any_of(clusters.begin(), clusters.end(), [](const std::set<Node*>& s){ return !s.empty(); });//clusters不为空时true,可改进
      if(hasNonEmptyClusters){
        int _Node_Num2 = nodes.size();
        aggregrate_Nodes(clusters, nodes,HyperGraph,area);
        _Node_Num = nodes.size();
        hash_num=_Node_Num2-_Node_Num>level?hash_num:hash_num-1;
        if(hash_num<1){
          hash_num=1;
        }
      }
      else{
        hash_num=hash_num-1;
        hash_num=hash_num<1?1:hash_num;
      }
    } 
    std::queue<Hyperedge*> q;
    for(auto edge:edges){//去除多余边   
      if(edge->nodes.size()==1){
        q.push(edge);
      }
    }
    while (!q.empty()) {
      auto edge = q.front();
      q.pop();
      edges.erase(edge);
    }
    HyperGraph.update(nodes,edges);

}



//解一层稀疏化
HyperGraph desparse_Nodes(HyperGraph& _HyperGraph){
  NodeSet nodes = _HyperGraph.Node_vector;
  HyperedgeSet edges = _HyperGraph.Edge_vector;
  NodeSet nodes_de=nodes;
  for(auto node:nodes){
    if(!node->Inclusion_node.empty()){
      for(auto desparse_node:node->Inclusion_node){//添加inclusion里的node和edge
        node->fpga->desparse_Node(node);//zwl添加

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
    nodes.insert(node);
  }
  HyperGraph HyperGraph_de(nodes,edges);
  return HyperGraph_de;
}
HyperGraph desparse_Node(HyperGraph& _HyperGraph,Node* node){
  NodeSet nodes = _HyperGraph.Node_vector;
  HyperedgeSet edges = _HyperGraph.Edge_vector;
  NodeSet nodes_de;
  for(auto node:nodes){
    nodes_de.insert(node);
  }
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
  for(auto node:nodes_de){//edge解稀疏化
    for(auto edge:node->hyperedges){
      if(edge->nodes.size()!=1){
        edges.insert(edge);
      }
    }
  }
  nodes.clear();
  for(auto node:nodes_de){
    nodes.insert(node);
  }
  HyperGraph HyperGraph_de(nodes,edges);
  return HyperGraph_de;
}

bool allInclusionNodesEmpty(const NodeSet& nodes) {//判断nodes是否Inclusion_node
    for (const auto* node : nodes) {
        if (!node->Inclusion_node.empty()) {
            return false;
        }
    }
    return true;
}

void desparseHypergraph(HyperGraph& _HyperGraph){//解稀疏化到原来两倍
  NodeSet& nodes = _HyperGraph.Node_vector;
  HyperedgeSet& edges = _HyperGraph.Edge_vector;
  HyperGraph _HyperGraph1(nodes,edges);
  while((!allInclusionNodesEmpty(_HyperGraph1.Node_vector))&(_HyperGraph1.Node_vector.size()<(2*nodes.size()))){
    _HyperGraph1=desparse_Nodes(_HyperGraph1);
  }
  _HyperGraph=_HyperGraph1;
}
void desparseHypergraph_all(HyperGraph& _HyperGraph){
  NodeSet& nodes = _HyperGraph.Node_vector;
  HyperedgeSet& edges = _HyperGraph.Edge_vector;
  HyperGraph _HyperGraph1(nodes,edges);
  while(!allInclusionNodesEmpty(_HyperGraph1.Node_vector)){
    _HyperGraph1=desparse_Nodes(_HyperGraph1);
  }
  _HyperGraph=_HyperGraph1;
}
void  desparNodeset(HyperGraph& _HyperGraph){
  NodeVector nodevec;
  NodeSet& nodes = _HyperGraph.Node_vector;
  HyperedgeSet& edges = _HyperGraph.Edge_vector;
  for(auto node:nodes){
    if(node->fpga==nullptr){
      nodevec.push_back(node);
    }
  }
  HyperGraph _HyperGraph1(nodes,edges);
  for (auto node : nodevec) {
    _HyperGraph1=desparse_Node(_HyperGraph1,node);
  }
  _HyperGraph=_HyperGraph1;
}

//nodes全部解稀疏化
void desparseNodes(NodeSet& nodes){
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
        nodes.insert(node);
      }
  }
}

//强制聚合顶点
void aggregrate_Nodes_force(HyperGraph& HyperGraph,int area,int node_limit){
  NodeSet& nodes_ori = HyperGraph.Node_vector;
  HyperedgeSet& edges=HyperGraph.Edge_vector;
  ConstraintChecker checker;
  int nodes_num=nodes_ori.size();
  int i=0;
  while(nodes_num > node_limit){
    auto node=nodes_ori.begin();
    while((*node)->area.values[0] > area){
      node++;
    }
      int id=nodes_ori.size();
      Node* node_new = new Node(id);

      auto node_neis=(*node)->getneiNode();
      node_new->addNode(*node);
      i++;
      std::cout <<  i <<  std::endl;
      nodes_ori.erase(node);
      for(auto node_nei:node_neis){
        if(node_nei->area.values[0] < area){
          node_new->addNode(node_nei);
          i++;
          std::cout <<  i <<  std::endl;
          nodes_ori.erase(node_nei);
          nodes_num=nodes_ori.size();
        }
        if(node_new->area.values[0] > area){
          break;
        }
      }
      node_new->update_hyperedges_less();
      nodes_ori.insert(node_new);
      HyperGraph.Node_vector_all.push_back(node_new);
      nodes_num=nodes_ori.size();
  }
  std::queue<Hyperedge*> q;
  for(auto edge:edges){//去除多余边   
    if(edge->nodes.size()==1){
      q.push(edge);
    }
  }
  while (!q.empty()) {
    auto edge = q.front();
    q.pop();
    edges.erase(edge);
  }
  HyperGraph._NumNode=nodes_ori.size();
  HyperGraph._NumEdge=edges.size();
}


bool aggregrate_Nodes2(std::vector<std::set<Node*>> clusters,HyperGraph& HyperGraph,int area){
    NodeSet& nodes = HyperGraph.Node_vector;
    int i=0;
    for (auto cluster : clusters) {
      auto it = cluster.begin();
      NodeVector nodedel;
      while (it != cluster.end()) {
        auto it2 = std::next(it); // 获取下一个迭代器
        if((*it)->area.values[0] > area){
          it++;          
        }
        else{
          if(it2==cluster.end()){
            break;
          }
          int id=HyperGraph.Node_vector_all.size();
          Node* node = new Node(id);
          // 使用一个独立的迭代器进行内层循环
          auto inner_it = it;
          while (inner_it != cluster.end()) {
              node->addNode(*inner_it);  // 累加值
              i++;
              std::cout <<  i <<  std::endl;
              nodes.erase(*inner_it);  // 从Node_all中删除
              if (node->area.values[0] > area) {
                ++inner_it;
                  break;  // 如果面积超过 500，停止
              }
              ++inner_it;  // 移动到下一个元素
          }
          HyperGraph.Node_vector_all.push_back(node);  // 加入All_node
          nodes.insert(node);  // 加入Node_all
          node->update_hyperedges_less();
          NodeSet a=node->getneiNode();
          it = inner_it;  // 更新外层迭代器
        }
      } 
    }
    if(i==0){
      return false;
    }
    std::queue<Hyperedge*> q;
    for(auto edge:HyperGraph.Edge_vector){//去除多余边   
      if(edge->nodes.size()==1){
        q.push(edge);
      }
    }
    while (!q.empty()) {
      auto edge = q.front();
      q.pop();
      HyperGraph.Edge_vector.erase(edge);
    }
    HyperGraph._NumNode=nodes.size();
    HyperGraph._NumEdge=HyperGraph.Edge_vector.size();
    return true;
    
}
void aggregrate_Nodes_force2(HyperGraph& HyperGraph,int area,int node_limit){
  while(HyperGraph._NumNode > node_limit){
      std::vector<NodeSet> clusters;
      auto it=HyperGraph.Edge_vector.begin();
      clusters.push_back((*it)->nodes);
      while(true){
        bool a=aggregrate_Nodes2(clusters,HyperGraph,area);
        if(a==true){
          break;
        }
        clusters.clear();
        it++;
        clusters.push_back((*it)->nodes);
      }
  }
}


void aggregrate_Nodes3(std::vector<std::vector<Node*>> clusters,HyperGraph& HyperGraph,int area){
    NodeSet& nodes = HyperGraph.Node_vector;
    int i=0;
    for (auto cluster : clusters) {
      auto it = cluster.begin();
      NodeVector nodedel;
      while (it != cluster.end()) {
        if((*it)->area.values[0] > area){
          it++;          
        }
        else{
          int id=HyperGraph.Node_vector_all.size();
          Node* node = new Node(id);
          // 使用一个独立的迭代器进行内层循环
          auto inner_it = it;
          while (inner_it != cluster.end()) {
              node->addNode(*inner_it);  // 累加值
              i++;
              std::cout <<  i <<  std::endl;
              nodes.erase(*inner_it);  // 从Node_all中删除
              if (node->area.values[0] > area) {
                ++inner_it;
                  break;  // 如果面积超过 500，停止
              }
              ++inner_it;  // 移动到下一个元素
          }
          HyperGraph.Node_vector_all.push_back(node);  // 加入All_node
          nodes.insert(node);  // 加入Node_all
          node->update_hyperedges_less();
          NodeSet a=node->getneiNode();
          it = inner_it;  // 更新外层迭代器
        }
      } 
    }
    std::queue<Hyperedge*> q;
    for(auto edge:HyperGraph.Edge_vector){//去除多余边   
      if(edge->nodes.size()==1){
        q.push(edge);
      }
    }
    while (!q.empty()) {
      auto edge = q.front();
      q.pop();
      HyperGraph.Edge_vector.erase(edge);
    }
    HyperGraph._NumNode=nodes.size();
    HyperGraph._NumEdge=HyperGraph.Edge_vector.size();
    
}
void aggregrate_Nodes_force3(HyperGraph& HyperGraph,int area,int node_limit){
  while(HyperGraph._NumNode > node_limit){
    std::vector<NodeVector> clusters;
    NodeVector cluster;
    auto startnode=HyperGraph.Node_vector.begin();
    while((*startnode)->area.values[0] > area){
      startnode++;
    }
    cluster.push_back(*startnode);
    for(auto node:(*startnode)->getneiNode()){
      cluster.push_back(node);
    }
    clusters.push_back(cluster);
    aggregrate_Nodes3(clusters,HyperGraph,area);
  }
}

void aggregrate_Nodes_force4(HyperGraph& HyperGraph,int area){
    std::vector<NodeSet> clusters;
    NodeSet cluster;
    NodeSet& nodes = HyperGraph.Node_vector;
    auto maxnode = std::max_element(nodes.begin(), nodes.end(), [](Node* a, Node* b) {
          return a->hyperedges.size() < b->hyperedges.size(); // 比较节点数
    });
    
    auto startnode=maxnode;
    for(auto node:(*startnode)->getneiNode()){
      cluster.insert(node);
    }
    clusters.push_back(cluster);
    aggregrate_Nodes2(clusters,HyperGraph,area);
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
