


class ConstraintChecker {
public:
    void check(FpgaVector &fpgas, HyperGraph &hypergraph) {
        HyperedgeSet &hyperedges = hypergraph.Edge_vector;
        for (auto edge : hyperedges) {
            FpgaMap fpgaCount = edge->getFpgaMap();
            // Check if the edge is assigned to more than one FPGA
            if (fpgaCount.size() > 1) {
                for(auto fpga : fpgas) {
                    fpga->nowcoppoints+=edge->weight;
                }
            }
            for (auto node : edge->geterror_nodes(this->maxdistance)) {
                error_nodes.insert(node);
            }  
            erroredges.insert(edge);
        }
        for (auto fpga : fpgas) {
            if (fpga->nowcoppoints > fpga->maxcoppoints) {
                errorfpgas.push_back(fpga);
            }
        }
        if (error_nodes.empty() && erroredges.empty() && errorfpgas.empty()) {
            Satisfy = true;
        }
    }
    // void fix_erroredges(FpgaVector &fpgas, HyperGraph &hypergraph) {
    //     for (auto edge : erroredges) {
    //         FpgaMap fpgaCount = edge->getFpgaMap();
    //         Fpga* src_fpga=edge->src_node->fpga;
    //         int max_distance=0;
    //         for (auto fpga:fpgaCount){
    //             if (fpga.first!=src_fpga){
    //                 int distance=dis_fpgas(src_fpga,fpga.first);
    //                 max_distance=distance>max_distance?distance:max_distance;
    //             }
    //         }
    //         if (max_distance > maxdistance) {
    //             for (auto fpga : fpgas) {
    //                 if (fpgaCount[fpga] == 1) {
    //                     fpgaCount[fpga] = 0;
    //                     fpgaCount[src_fpga] = 1;
    //                     break;
    //                 }
    //             }
    //         }
    //     }
    //     for (auto fpga : errorfpgas) {
    //         fpga->nowcoppoints = 0;
    //         fpga->areaconstrain = false;
    //         fpga->copconstrain = false;
    //     }
    // }
    bool Satisfy=false;
    int maxdistance;
    NodeSet error_nodes;
    HyperedgeSet erroredges;
    FpgaVector errorfpgas;

};