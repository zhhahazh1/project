#include <cstdint>
#include <iostream>
#include <class.h>
#include <read.h>
#include <sparsification.h>

int main(){
    std::string design_aera = "../exapmle/case01/design.are";
    std::string design_net = "../exapmle/case01/design.net";
    std::string design_info = "../exapmle/case01/design.info";
    std::string design_topo = "../exapmle/case01/design.topo";
    NodeVector nodes = readNodes(design_aera);
    HyperedgeSet Hyperedge =readEdges(design_net, nodes);
    HyperGraph HyperGraph(nodes,Hyperedge);                                                                                                                                                                                     
    buildSparsifiedHypergraph(HyperGraph,3);
    desparseHypergraph(HyperGraph);
    return 0;
    }
