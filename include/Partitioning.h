#include <limits>
bool checkcon(Node* node,Fpga* tarfpga,ConstraintChecker &checker){
    HyperedgeSet neiedges = node->hyperedges;
    //边约束检查
    for (auto edge:neiedges){
        if(node==edge->src_node){
            FpgaMap fpgacount=edge->fpgaCount;//复制一份fpgacount
            fpgacount[node->fpga]--;
            for (auto fpga:fpgacount){
                if (fpga.first!=tarfpga){
                    int distance=dis_fpgas(tarfpga,fpga.first);
                    if (distance>checker.maxdistance){
                        return false;
                    }
                }
            }
        }
        else{
            int distance =dis_fpgas(tarfpga,edge->src_node->fpga);
            if (distance>checker.maxdistance){
                        return false;
            }
        }
        
    }
    //面积约束检查
    if (tarfpga->usearea+node->area>tarfpga->area){
        return false;
    }
    //检查目标fpga的cop
    int tarfpga_nowcop=tarfpga->nowcoppoints;
    for (auto edge:neiedges){
        FpgaMap fpgacount=edge->fpgaCount;//复制一份fpgacount
        fpgacount[tarfpga]+=1;
        if(fpgacount.size()>1&&fpgacount[tarfpga]==1){//感觉可以改成if(fpgacount[tarfpga]==1)
            tarfpga_nowcop+=edge->weight;
            if (tarfpga_nowcop>tarfpga->maxcoppoints){
                return false;
            }
        }
        
    }
    //检查当前fpga的cop
    int norfpga_nowcop=node->fpga->nowcoppoints;
    for (auto edge:neiedges){
        FpgaMap fpgacount=edge->fpgaCount;//复制一份fpgacount
        if(fpgacount.size()==1){
            norfpga_nowcop+=edge->weight;
            if (norfpga_nowcop>node->fpga->maxcoppoints){
                return false;
            }
        }
        
    }
    return true;
}

void Point (Node* node,ConstraintChecker &checker){
    node->gain.clear();
    node->maxgain = {nullptr, 0};
    HyperedgeSet neiedges = node->hyperedges;
    Fpga* nor_fpga=node->fpga;//移动前位置
    FpgaMap gain;//最后得到的东西
    FpgaVector tarfpgas=node->getneifpga();//移动目标范围
    for (auto edge:neiedges){
        FpgaMap fpgas=edge->fpgaCount;//这条边所包含的fpga
        Fpga* src_fpga=edge->src_node->fpga;//边的源节点
        int points0=edge->points;//边的初始分数
        //计算移动后的分数
        if (node==edge->src_node){
            for (auto tar_fpga:tarfpgas){
                if(checkcon(node,tar_fpga,checker)){  
                    fpgas[nor_fpga]--;
                    fpgas[tar_fpga]++;
                    if (fpgas[nor_fpga] == 0) {
                        fpgas.erase(nor_fpga);
                    }//更新移动后的边占fpga
                    int points1=0;
                    for (auto fpga2:fpgas){
                        points1 += dis_fpgas(tar_fpga,fpga2.first);   
                    }//计算移动后分数
                    gain[tar_fpga]+=points0-points1;              
                }
            };
        }//移动节点是源节点情况时，任何情况都要重新计算
        else{
            for (auto tar_fpga:tarfpgas){
                if(checkcon(node,tar_fpga,checker)){
                    auto it = fpgas.find(tar_fpga);
                    if(fpgas[nor_fpga]==1){//块和节点边交集大于1
                        gain[tar_fpga]=dis_fpgas(src_fpga,nor_fpga)-dis_fpgas(src_fpga,tar_fpga);
                    }
                    else if(it == fpgas.end()){//目标块和当前边无交集
                        gain[tar_fpga]=dis_fpgas(src_fpga,tar_fpga);
                    }
                    else{
                        gain[tar_fpga]=0;
                    }
                }
                
            }
        }
    }
    auto maxgain = std::max_element(gain.begin(), gain.end(),
        [](const std::pair<Fpga*, int>& a, const std::pair<Fpga*, int>& b) {
            return a.second > b.second;
    });
    node->gain=gain;
    if(maxgain!=gain.end()){
        node->maxgain = (*maxgain);
    }
}

void delnode_GainFpgaMap(Node* node,GainFpgaMap &gainFpgamap){
    int gain = node->maxgain.second;
    auto it = gainFpgamap.find(gain);
    if (it != gainFpgamap.end()) {
        // 获取对应的节点列表
        auto &nodeList = it->second;
        //从nodeset中删除指定节点
        nodeList.erase(node);
        node->movenable=false;
        // 如果删除节点后，节点列表为空，则删除该增益值对应的 entry
        if (nodeList.empty()) {
            gainFpgamap.erase(it);
        }
    }
}

void update_GainFpgaMap (Node* node,int oldgain, GainFpgaMap &gainFpgamap){
    auto it = gainFpgamap.find(oldgain);
    if (it != gainFpgamap.end()) {
        // 找到旧增益值对应的节点列表
        auto &nodeList = it->second;

        // 从节点列表中删除该节点
        nodeList.erase(node);
        // 如果该增益值的节点列表为空，则移除该增益值
        if (nodeList.empty()) {
            gainFpgamap.erase(it);
        }
    }
    // 2. 更新节点的增益值
    int newGain=node->maxgain.second;

    // 3. 将节点插入新的增益值列表
    gainFpgamap[newGain].insert(node);

}

// void updateNode(Node* node,Fpga* tarfpga){
//     node->fpga=tarfpga;
// }

// void updateEdge(Hyperedge* edge,Fpga* oldfpga,Fpga* tarfpga){
//         //更新边的同时更新fpga的nowcoppoints
//         FpgaMap &edgefpgas = edge->fpgaCount;
//         edgefpgas[oldfpga]--;
//         edgefpgas[tarfpga]++;
//         if (edgefpgas[oldfpga] == 0) {
//             edgefpgas.erase(oldfpga);
//             oldfpga->nowcoppoints-=edge->weight;
//         }
//         if (edgefpgas[tarfpga] == 1) {
//             oldfpga->nowcoppoints-=edge->weight;
//         }
        
// }
void updateEdge_points(Hyperedge* edge){
    Fpga* src_fpga=edge->src_node->fpga;//边的源节点
    FpgaMap fpgas=edge->fpgaCount;
    int points0=0;
    for (auto fpga:fpgas){
        points0+=edge->weight*dis_fpgas(src_fpga,fpga.first);
    }
    edge->points=points0;
}
// void updateFpga(Node* node,Fpga* oldfpga,Fpga* tarfpga){
//     oldfpga->nodes.erase(node);
//     tarfpga->add_node(node);
// }

int Move (Node* node,GainFpgaMap &gainFpgamap,ConstraintChecker &checker){
    if(!checkcon(node,node->maxgain.first,checker)){
        int maxpoints = 0;
        int oldgain=node->maxgain.second;
        Point(node,checker);
        update_GainFpgaMap(node,oldgain,gainFpgamap);
        return node->maxgain.second;
    }
    Fpga* oldfpga = node->fpga;
    Fpga* tarfpga=node->maxgain.first;
    NodeSet neinodes = node->getneiNode();
    oldfpga->erase_node(node);
    tarfpga->add_node(node);
    for(auto edge:node->hyperedges){
        updateEdge_points(edge);
    }
    node->movenable=false;
    delnode_GainFpgaMap(node,gainFpgamap);
    int maxpoints = 0;
    for(auto neinode:neinodes){
        if(neinode->movenable!=false){
            int oldgain=neinode->maxgain.second;
            neinode->gain.clear();
            Point(neinode,checker);
            maxpoints=neinode->maxgain.second>maxpoints?neinode->maxgain.second:maxpoints;
            update_GainFpgaMap(neinode,oldgain,gainFpgamap);
        }
        
    }
    return maxpoints;
}




void addpoints (HyperGraph &HyperGraph){
    HyperedgeSet &edges=HyperGraph.Edge_vector;

    for(auto edge:edges){
        Fpga* src_fpga=edge->src_node->fpga;//边的源节点
        FpgaMap fpgas=edge->fpgaCount;
        int points0=0;
        for (auto fpga:fpgas){
            points0+=edge->weight*dis_fpgas(src_fpga,fpga.first);
        }
        edge->points=points0;
    }
}

void Partitioning(HyperGraph &HyperGraph,FpgaVector& fpgas,ConstraintChecker &checker){
    addpoints (HyperGraph);
    NodeVector &Nodes =HyperGraph.Node_vector;
    int maxgain=0;
    GainFpgaMap gainFpgamap;
    for (auto node:Nodes){
        Point(node,checker);
        if(node->maxgain.first != nullptr && node->maxgain.second != 0){
            int gain=node->maxgain.second;        
            gainFpgamap[gain].insert(node);
            maxgain=gain>maxgain?gain:maxgain;
        }
    }    
    while (maxgain>0){
        Node* movenode=*(gainFpgamap[maxgain].begin());
        int newpoints=Move(movenode,gainFpgamap,checker);
        maxgain=newpoints>maxgain?newpoints:maxgain;
        while(gainFpgamap.find(maxgain)==gainFpgamap.end()){
            maxgain--;
        }
        // checker.clean();
        // checker.check(fpgas,HyperGraph);
        int a=0;
    }
}



