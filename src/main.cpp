
#include <cstdlib>  // 包含rand()和srand()
#include <ctime>    // 包含time()
#include <stdexcept>  // 包含标准异常库
#include <random>
#include <limits>
#include <mutex>
#include <thread>
#include <future>
std::mutex seed_mutex;
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
    unsigned int initial_seed = 12345678; // 初始种子
    
    std::string desing_aera = "../exapmle/case01/design.are";
    std::string desing_net = "../exapmle/case01/design.net";
    std::string desing_info = "../exapmle/case01/design.info";
    std::string desing_topo = "../exapmle/case01/design.topo";
    NodeVector nodes = readNodes(desing_aera);
    HyperedgeSet Hyperedge = readEdges(desing_net, nodes);
    HyperGraph hyperGraph(nodes, Hyperedge);
    FpgaVector fpgas = readFpgas(desing_info);
    int hop_max;
    ConstraintChecker checker;
    readtopo(desing_topo, checker, fpgas);
    
    Initial6::PartitionResult result =Initial6::multinitial(hyperGraph, fpgas, checker, initial_seed, 10);
    HyperGraph* in_hyperGraph = result.hypergraph;
    FpgaVector* in_fpgas = result.fpgas;
    int hasinitial=0;
    for(auto fpga: *in_fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;

    Partitioning(*in_hyperGraph,*in_fpgas,checker);
    hasinitial=0;
    for(auto fpga: *in_fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    // Initial6::InitialPartitioning(*hypergraphs[0],*fpgasvector[0],checker,seeds[0]);
    

    // 等待所有线程执行完毕
    

    return 0;
}
