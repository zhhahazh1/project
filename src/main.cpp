#include <class.h>
#include<ConCleck.h>
#include <read.h>
#include<Initial.h>
// #include<sparsification.h>

// #include<Partitioning.h>
int main() {
    std::string desing_aera = "../exapmle/case01/design.are";
    std::string desing_net = "../exapmle/case01/design.net";
    std::string desing_info = "../exapmle/case01/design.info";
    std::string desing_topo = "../exapmle/case01/design.topo";
    NodeVector nodes = readNodes(desing_aera);

    HyperedgeSet Hyperedge =readEdges(desing_net, nodes);
    
                                                                                                                                                                                                                      
    HyperGraph HyperGraph(nodes,Hyperedge);
    FpgaVector fpgas=readFpgas(desing_info);
    int hop_max;
    ConstraintChecker checker;
    readtopo(desing_topo,checker,fpgas);
    InitialPartitioning(HyperGraph,fpgas);
    // Partitioning(HyperGraph,fpgas,5);
    return 0;
}
