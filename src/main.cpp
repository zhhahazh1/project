#include <class.h>
#include <read.h>
#include<Initial.h>
#include<Partitioning.h>
int main() {
    std::string desing_aera = "../exapmle/design.are";
    std::string desing_net = "../exapmle/design.net";
    NodeVector nodes = readNodes(desing_aera);

    HyperedgeVector Hyperedge =readEdges(desing_net, nodes);
    HyperGraph HyperGraph(nodes,Hyperedge);
    FpgaVector fpgas(4,nullptr);
    InitialPartitioning(HyperGraph,fpgas);
    Node node1=*nodes[0];
    FpgaVector node1_neifpgas=node1.getneifpga();
    // Partitioning(HyperGraph,fpgas,5);
    return 0;
}
