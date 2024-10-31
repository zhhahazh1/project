
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
#include<sparsification.h>
#include<Initial6.h>
#include<Partitioning.h>
#include<write.h>

int hasinitial=0;
int main(int argc, char *argv[]) {
    unsigned int initial_seed = 12345678; // 初始种子
    //unsigned int initial_seed = static_cast<unsigned int>(time(0));
    std::cout << "Seed used: " << initial_seed << std::endl;
    std::string desing_aera = "../exapmle/case03/design.are";
    std::string desing_net = "../exapmle/case03/design.net";
    std::string desing_info = "../exapmle/case03/design.info";
    std::string desing_topo = "../exapmle/case03/design.topo";
    std::string outputFile1 = "design.are";
    std::string outputFile2 = "design.net";
    std::string outputFile = "output.txt";
    NodeVector nodes = readNodes(desing_aera);
    HyperedgeSet Hyperedge = readEdges(desing_net, nodes);
    HyperGraph hyperGraph(nodes, Hyperedge);
    FpgaVector fpgas = readFpgas(design_info);
    int hop_max;
    int switch_node=nodes.size();
    ConstraintChecker checker;
    readtopo(desing_topo, checker, fpgas);
    //buildSparsifiedHypergraph(hyperGraph,10,10000,500);
    // buildSparsifiedHypergraph(hyperGraph,10,1000,10000);
    // buildSparsifiedHypergraph(hyperGraph,10,100,10000);
    // write2(hyperGraph,outputFile1,outputFile2);
    //buildSparsifiedHypergraph2(hyperGraph,10,100,2147483647);
    //Initial6::PartitionResult result =Initial6::multinitial(hyperGraph, fpgas, checker, initial_seed, 1);
    Initial6::InitialPartitioning(hyperGraph, fpgas,checker,1055721139);
    HyperGraph* in_hyperGraph = &hyperGraph;
    FpgaVector* in_fpgas = &fpgas;
    bool a=checker.checkmaxcop(*in_fpgas,*in_hyperGraph);
    int hasinitial=0;
    for(auto fpga: *in_fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    checker.checkarea(*in_fpgas);
    Partitioning(*in_hyperGraph,*in_fpgas,checker);
    hasinitial=0;
    for(auto fpga: *in_fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    checker.checkarea(*in_fpgas);

    desparseHypergraph(*in_hyperGraph);
    checker.checkgrapy(*in_hyperGraph);
    in_hyperGraph->update_edge_fpgacont();
    in_hyperGraph->remove();
    Partitioning(*in_hyperGraph,*in_fpgas,checker);
    hasinitial=0;
    for(auto fpga: *in_fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    checker.checkarea(*in_fpgas);

    desparseHypergraph_all(*in_hyperGraph);
    in_hyperGraph->update_edge_fpgacont();
    in_hyperGraph->remove();
    Partitioning(*in_hyperGraph,*in_fpgas,checker);
    hasinitial=0;
    for(auto fpga: *in_fpgas){
        fpga->print();
        hasinitial+=fpga->nodes.size();
    }        
    std::cout << "hasinitial:" << hasinitial << std::endl;
    checker.checkarea(*in_fpgas);
    writhe(*in_fpgas,outputFile);
    // 等待所有线程执行完毕
    

    return 0;
}
