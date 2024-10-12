using namespace Initial6;
    void con_initial(HyperGraph &HyperGraph, FpgaVector &Fpgas,ConstraintChecker &checker){
        NodeVector &nodes=HyperGraph.Node_vector;
        for(Node* node : nodes){
            if(node->fpga == nullptr){
                
                for(auto fpga:node->getneifpga()){
                    if(checkadd(node,fpga,checker)){ 
                        fpga->add_node(node);
                    }
                }

            }
        }
    }