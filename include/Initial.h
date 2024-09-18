#include <cmath>
void InitialPartitioning(HyperGraph &HyperGraph,FpgaVector &Fpgas){
    int totalNodes = HyperGraph._NumNode;
    int numFpgas = Fpgas.size();
    int minSize = totalNodes / numFpgas;
    int extraElements = totalNodes % numFpgas;
    int sqrtNumFpgas = std::sqrt(numFpgas);
    NodeVector& nodes=HyperGraph.Node_vector;
    int start = 0;
    int ID[2]={0,0};
    for (int i = 0; i < numFpgas-1; ++i) {
        int end = start + minSize;
        ID[0]= i / sqrtNumFpgas;
        ID[1]= i % sqrtNumFpgas;
        Fpga* fpga=new Fpga(ID);
        for (auto it = nodes.begin() + start; it != nodes.begin() + end; ++it) {
            fpga->add_node(*it);
            (*it)->fpga = fpga;
            for(auto edge:(*it)->hyperedges){
                edge->fpgaCount[fpga]+=1;
            }
        }
        (Fpgas)[i] = fpga;
        start = end;
    }
    int end = start + minSize+extraElements;
    ID[0] = sqrtNumFpgas - 1; 
    ID[1] = sqrtNumFpgas - 1; 
    Fpga* fpga=new Fpga(ID);
    for (auto it = nodes.begin() + start; it != nodes.begin() + end; ++it) {
        fpga->add_node(*it);
        (*it)->fpga = fpga;
        for(auto edge:(*it)->hyperedges){
            edge->fpgaCount[fpga]+=1;
        }
    }
    (Fpgas)[numFpgas-1] = fpga;
}