
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
    std::unordered_map<size_t, Node*> node_map;
    std::cout << "Seed used: " << initial_seed << std::endl;
    std::string desing_aera = "../exapmle/case04/design.are";
    std::string desing_net = "../exapmle/case04/design.net";
    std::string desing_info = "../exapmle/case04/design.info";
    std::string desing_topo = "../exapmle/case04/design.topo";
    std::string outputFile1 = "design.are";
    std::string outputFile2 = "design.net";
    std::string outputFile = "output.txt";
    NodeVector nodes = readNodes(desing_aera,node_map);
    HyperedgeSet Hyperedge = readEdges(desing_net, node_map);
    HyperGraph hyperGraph(nodes, Hyperedge);
    FpgaVector fpgas = readFpgas(desing_info);
    ConstraintChecker checker;
    readtopo(desing_topo, checker, fpgas);
    
    buildSparsifiedHypergraph(hyperGraph,10,2,10000,4);
    write2(hyperGraph,outputFile1,outputFile2);
    buildSparsifiedHypergraph(hyperGraph,10,2,5000,12);
    write2(hyperGraph,outputFile1,outputFile2);
    buildSparsifiedHypergraph(hyperGraph,10,2,2500,26);
    write2(hyperGraph,outputFile1,outputFile2);
    buildSparsifiedHypergraph(hyperGraph,10,2,1000,60);
    write2(hyperGraph,outputFile1,outputFile2);
    buildSparsifiedHypergraph(hyperGraph,10,2,1000,140);
    write2(hyperGraph,outputFile1,outputFile2);
    buildSparsifiedHypergraph(hyperGraph,10,2,500,300);
    write2(hyperGraph,outputFile1,outputFile2);
    buildSparsifiedHypergraph(hyperGraph,10,2,200,620);
    write2(hyperGraph,outputFile1,outputFile2);
    buildSparsifiedHypergraph(hyperGraph,10,2,100,1000);
    write2(hyperGraph,outputFile1,outputFile2);
    buildSparsifiedHypergraph(hyperGraph,10,2,100,2000);
    write2(hyperGraph,outputFile1,outputFile2);
    buildSparsifiedHypergraph(hyperGraph,10,2,100,4000);
    write2(hyperGraph,outputFile1,outputFile2);
    // checker.checkgrapy(hyperGraph);
    // aggregrate_Nodes_force4(hyperGraph,5000);
    // buildSparsifiedHypergraph(hyperGraph,10,100,10000);
    // checker.checkgrapy(hyperGraph);
    // buildSparsifiedHypergraph(hyperGraph,10,1000,10000);
    // buildSparsifiedHypergraph(hyperGraph,1,100,10000);
    //write2(hyperGraph,outputFile1,outputFile2);
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
