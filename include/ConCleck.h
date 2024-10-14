class ConstraintChecker {
public:
    void check(FpgaVector &fpgas, HyperGraph &hypergraph) {
        HyperedgeSet &hyperedges = hypergraph.Edge_vector;
        for(auto fpga:fpgas){
            fpga->nowcoppoints=0;
        }
        for (auto edge : hyperedges) {
            FpgaMap fpgaCount = edge->getFpgaMap();
            // Check if the edge is assigned to more than one FPGA
            if (fpgaCount.size() > 1) {
                for(auto fpga : fpgaCount) {
                    fpga.first->nowcoppoints+=edge->weight;
                }
            }
            NodeSet errornodes = edge->geterror_nodes(this->maxdistance);
            for (auto node : edge->geterror_nodes(this->maxdistance)) {
                error_nodes.insert(node);
                erroredges.insert(edge);
            }  
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
    bool checkadd(Node* node,Fpga* fpga,ConstraintChecker &checker){
        if ((node->fpga == nullptr)&&(fpga->usearea + node->area <= (fpga->area)*1)){
            for (auto& edge : node->hyperedges) {
                if(edge->src_node->fpga!=nullptr){
                    if (dis_fpgas(edge->src_node->fpga,fpga)>checker.maxdistance){
                        return false;
                    }
                }
                else{
                    for (auto& neinode : edge->nodes) {
                        if (neinode->fpga!=nullptr){
                            if (dis_fpgas(neinode->fpga,fpga)>checker.maxdistance){
                                return false;
                            }
                        }
                    }
                }
            }
            //检查目标fpga的nowcoppoints是否大于maxcoppoints
            // FpgaMap fpgaMap;
            // fpgaMap[fpga] = fpga->nowcoppoints;
            // for(auto edge:node->hyperedges){
            //     FpgaMap fpgaCount=edge->fpgaCount;
            //     fpgaCount[fpga]+=1;
            //     if(fpgaCount.size()>1&&fpgaCount[fpga]==1){
            //         fpgaMap[fpga]+=edge->weight;
            //         if (fpgaMap[fpga]>fpga->maxcoppoints){
            //             return false;
            //         }
            //     }
            //     //此后这条边被切割了，把原本的fpga的coppoints加上
            //     if(fpgaCount.size()==2&&fpgaCount[fpga]==1){
            //         for(auto fpga2:fpgaCount){
            //             if(fpga2.first!=fpga){
            //                 if(fpgaMap.find(fpga2.first)==fpgaMap.end()){
            //                     fpgaMap[fpga2.first]=fpga2.first->nowcoppoints;
            //                 }
            //                 fpgaMap[fpga2.first]+=edge->weight;
            //                 if(fpgaMap[fpga2.first]>fpga2.first->maxcoppoints){
            //                     return false;
            //                 }
            //             }
            //         }
            //     }
            // }
            return true;
        }
        return false;

    }
    void print_errorfpas(){
        std::cout<<Satisfy<< std::endl;
        for(auto fpga:errorfpgas){
            std::cout << "FPGA" << fpga->ID << " cop points exceed the maximum cop points" << std::endl;
        }
    }
    void clean() {
        error_nodes.clear();
        erroredges.clear();
        errorfpgas.clear();
        Satisfy = false;
    }
    bool Satisfy=false;
    int maxdistance;
    FpgaVector aer_errorfpgas;
    NodeSet error_nodes;
    HyperedgeSet erroredges;
    FpgaVector errorfpgas;

};