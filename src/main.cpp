#include <cstdlib>  // 包含rand()和srand()
#include <ctime>    // 包含time()
#include <stdexcept>  // 包含标准异常库
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
    std::string desing_aera = "../exapmle/case03/design.are";
    std::string desing_net = "../exapmle/case03/design.net";
    std::string desing_info = "../exapmle/case03/design.info";
    std::string desing_topo = "../exapmle/case03/design.topo";
    NodeVector nodes = readNodes(desing_aera);

    HyperedgeSet Hyperedge =readEdges(desing_net, nodes);
    
                                                                                                                                                                                                                      
    HyperGraph HyperGraph(nodes,Hyperedge);
    FpgaVector fpgas=readFpgas(desing_info);
    int hop_max;
    ConstraintChecker checker;
    readtopo(desing_topo,checker,fpgas);
    //Initial1::InitialPartitioning(HyperGraph,fpgas,checker);
    //Initial2::InitialPartitioning(HyperGraph,fpgas,checker);
    //Initial3::InitialPartitioning(HyperGraph,fpgas,checker);
    //Initial4::InitialPartitioning(HyperGraph,fpgas,checker);
    //Initial5::InitialPartitioning(HyperGraph,fpgas,checker);
    Initial6::InitialPartitioning(HyperGraph,fpgas,checker);
    //Initial7::InitialPartitioning(HyperGraph,fpgas,checker);//太慢，不可取
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
    return 0;
}
