
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