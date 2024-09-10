#include <class.h>

int main() {
    int area[8]={0,1,2,3,4,5,6,7};
    Node node1(area), node2(area), node3(area);
    Hyperedge hyperedge0;
    Hyperedge hyperedge1;

    hyperedge0.addNode(&node1);
    hyperedge0.addNode(&node2);
    hyperedge1.addNode(&node2);
    hyperedge0.addNode(&node3);
    std::vector<Node*> nodes = {&node1, &node2, &node3};
    std::vector<Hyperedge*> edges = {&hyperedge0, &hyperedge1};

    HyperGraph hypergraph(nodes, edges);



    return 0;
}
