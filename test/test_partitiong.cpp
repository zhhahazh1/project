#include <cstdlib>  // 包含rand()和srand()
#include <ctime>    // 包含time()
#include <random>
#include <class.h>
#include<ConCleck.h>
#include <read.h>
#include<Initial1.h>
#include<Initial2.h>
#include<Initial3.h>
#include<Initial4.h>
#include<Initial5.h>
#include<Initial6.h>
#include<Initial7.h>
#include<con_initial.h>
// #include<sparsification.h>

#include<Partitioning.h>
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
    int node_indices1[] = {3,6,8,10,11,13};
    int node_indices2[] = {0,5,9,14};
    int node_indices3[] = {1,2,4,15};
    int node_indices4[] = {7,12};

    // 将指定的节点添加到 fpgas

    for (int index : node_indices1) {
        fpgas[0]->add_node(nodes[index]);
    }
    for (int index : node_indices2) {
        fpgas[1]->add_node(nodes[index]);
    }
    for (int index : node_indices3) {
        fpgas[2]->add_node(nodes[index]);
    }
    for (int index : node_indices4) {
        fpgas[3]->add_node(nodes[index]);
    }
    int hasinitial=0;
    checker.check(fpgas,HyperGraph);
    for(auto fpga: fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    checker.print_errorfpas();

    Partitioning(HyperGraph,fpgas,checker);
    hasinitial=0;
    checker.clean();
    checker.check(fpgas,HyperGraph);
    for(auto fpga: fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    checker.print_errorfpas();

    // Partitioning(HyperGraph,fpgas,checker);
    // hasinitial=0;
    // checker.clean();
    // checker.check(fpgas,HyperGraph);
    // for(auto fpga: fpgas){
    //     fpga->print();
    //     hasinitial+=fpga->nodes.size();
    // }        
    // std::cout << "hasinitial:" << hasinitial << std::endl;
    // checker.print_errorfpas();
    return 0;
}
