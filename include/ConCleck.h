class ConstraintChecker {
public:
    bool checkmaxcop(FpgaVector &fpgas, HyperGraph &hypergraph) {
        HyperedgeSet &hyperedges = hypergraph.Edge_vector;
        for (auto fpga : fpgas) {
            fpga->checkcoppoints = 0;
        }
        for (auto edge : hyperedges) {
            FpgaMap fpgaCount = edge->getFpgaMap();
            // Check if the edge is assigned to more than one FPGA
            if (fpgaCount.size() > 1) {
                for(auto fpga : fpgaCount) {
                    fpga.first->checkcoppoints+=edge->weight;
                }
            }
            // NodeSet errornodes = edge->geterror_nodes(this->maxdistance);
            // for (auto node : edge->geterror_nodes(this->maxdistance)) {
            //     error_nodes.insert(node);
            //     erroredges.insert(edge);
            // }  
        }
        for(auto fpga : fpgas) {
            if(fpga->checkcoppoints != fpga->nowcoppoints) {
                return false;
            }
        }
        return true;
    }
    bool checkgrapy(HyperGraph &hypergraph){
        for(auto node:hypergraph.Node_vector){
            for(auto edge:node->hyperedges_less){
                if(hypergraph.Edge_vector.find(edge)==hypergraph.Edge_vector.end()){
                    return false;
                }
                if(edge->nodes.find(node)==edge->nodes.end()){
                    return false;
                }
            }
        }
        for(auto edge:hypergraph.Edge_vector){
            for(auto node:edge->nodes){
                if(std::find(hypergraph.Node_vector.begin(),hypergraph.Node_vector.end(),node)==hypergraph.Node_vector.end()){
                    return false;
                }
            }
        }
        return true;
    }
    bool checkedge_fpgamap(HyperGraph &hypergraph){
        
        for(auto edge:hypergraph.Edge_vector){
            FpgaMap fpgaMap;
            for(auto node:edge->nodes){
                fpgaMap[node->fpga]+=1;
            }
            if(fpgaMap!=edge->fpgaCount){
                return false;
            }
        }
        return true;
    }
    void checkarea(FpgaVector &fpgas){
        errorfpgas.clear();
        for(auto fpga:fpgas){
            if(fpga->usearea>fpga->area){
                errorfpgas.push_back(fpga);
            }
        }
    }
    int check(FpgaVector &fpgas) {
        int cop=0;
        for (auto fpga : fpgas) {
            cop+=fpga->nowcoppoints;
        }
        return cop;
    }
    bool checkadd2(Node* node,Fpga* fpga,ConstraintChecker &checker){
        if ((node->fpga == nullptr)){
            for (auto& edge : node->hyperedges) {
                Node* source_node = edge->src_node.top();
                if(source_node->fpga!=nullptr){
                    if (dis_fpgas(source_node->fpga,fpga)>checker.maxdistance){
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
            return true;
        }
        return false;

    }
    bool checkadd(Node* node,Fpga* fpga,ConstraintChecker &checker){
        if ((node->fpga == nullptr)&&(fpga->usearea + node->area <= (fpga->area)*1)){
            for (auto& edge : node->hyperedges) {
                Node* source_node = edge->src_node.top();
                if(source_node->fpga!=nullptr){
                    if (dis_fpgas(source_node->fpga,fpga)>checker.maxdistance){
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