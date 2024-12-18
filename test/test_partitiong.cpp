#include <cstdlib>  // 包含rand()和srand()
#include <ctime>    // 包含time()
#include <stdexcept>  // 包含标准异常库
#include <random>
#include <limits>
#include <mutex>
#include <thread>
#include <future>


#include <my_time.h>
#include <class.h>
#include<ConCleck.h>
#include <read.h>
#include<Initial6.h>
#include<con_initial.h>
// #include<sparsification.h>

#include<Partitioning.h>
#include<write.h>
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
    int node_indices1[] = {1,4,5,6,7,10};
    int node_indices2[] = {0,2,3};
    int node_indices3[] = {8,9,11,13,14};
    int node_indices4[] = {12,15};

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
