#include <limits>

int dis_fpgas(Fpga* fpga1,Fpga* fpga2){
    int dietance=std::abs(fpga1->ID[0]-fpga2->ID[0])+std::abs(fpga1->ID[1]-fpga2->ID[1]);
    return dietance;
}

int findmindis(Fpga* fpga,FpgaMap fpgas){
    int distance = 0;
    int mindis=std::numeric_limits<int>::max();
    for (auto it:fpgas){
        distance=dis_fpgas(fpga,it.first);
        if (distance<mindis){
            mindis=distance;
        }
    }
    return mindis;
}

// void Partitioning(HyperGraph &HyperGraph,FpgaVector& fpgas,int max){
//     HyperedgeVector Hyperedge=HyperGraph.Edge_vector;
//     int maxgain=0;
//     for (auto edge:Hyperedge){
//         NodeVector nodes=edge->getSingleOccurrenceFPGANodes();
//         if(!nodes.empty()){
//             for(auto node:nodes){
//                 node->gain+=1;
//                 NodeVector neinodes = node->getneiNode();
//                 FpgaMap gains;
//                 for (auto neinode:neinodes){
//                     gains[neinode->fpga]+=dis_fpgas(node->fpga,neinode->fpga);
//                     int neggain =findmindis(neinode->fpga,edge->fpgaCount);
//                     gains[neinode->fpga]-=neggain;
//                 }
//             }
//         }
//     }
// }


void Partitioning(HyperGraph &HyperGraph,FpgaVector& fpgas,int max){
    NodeVector Nodes =HyperGraph.Node_vector;
    for (auto node:Nodes){
        HyperedgeVector neiedges = node->hyperedges;
        FpgaVector neifpgas = node->getneifpga();
    }
}

