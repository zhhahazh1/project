
#include <cstdlib>  // 包含rand()和srand()
#include <ctime>    // 包含time()
#include <stdexcept>  // 包含标准异常库
#include <random>
#include <my_time.h>
#include <class.h>
#include<ConCleck.h>
#include <read.h>
#include<Initial6.h>
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
    Initial6::InitialPartitioning(HyperGraph,fpgas,checker);
    int hasinitial=0;
    for(auto fpga: fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    
    con_initial(HyperGraph,fpgas,checker);
    hasinitial=0;
    for(auto fpga: fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    
    checker.check(fpgas,HyperGraph);
    hasinitial=0;
    for(auto fpga: fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    checker.print_errorfpas();

    Partitioning(HyperGraph,fpgas,checker);
    checker.clean();
    checker.check(fpgas,HyperGraph);
    hasinitial=0;
    for(auto fpga: fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    checker.print_errorfpas();
    
    // Partitioning(HyperGraph,fpgas,checker);
    // checker.clean();
    // checker.check(fpgas,HyperGraph);
    // hasinitial=0;
    // for(auto fpga: fpgas){
    //     fpga->print();
    //     hasinitial+=fpga->nodes.size();
    // }        
    // std::cout << "hasinitial:" << hasinitial << std::endl;
    // checker.print_errorfpas();
    stats.printStatistics();
    return 0;
}
