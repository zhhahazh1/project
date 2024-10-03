#include <limits>

// int dis_fpgas(Fpga* fpga1,Fpga* fpga2){
//     int dietance=std::abs(fpga1->ID[0]-fpga2->ID[0])+std::abs(fpga1->ID[1]-fpga2->ID[1]);
//     return dietance;
// }

// int findmindis(Fpga* fpga,FpgaMap fpgas){
//     int distance = 0;
//     int mindis=std::numeric_limits<int>::max();
//     for (auto it:fpgas){
//         distance=dis_fpgas(fpga,it.first);
//         if (distance<mindis){
//             mindis=distance;
//         }
//     }
//     return mindis;
// }

// void Partitioning(HyperGraph &HyperGraph,FpgaSet& fpgas,int max){
//     HyperedgeSet Hyperedge=HyperGraph.Edge_vector;
//     int maxgain=0;
//     for (auto edge:Hyperedge){
//         NodeSet nodes=edge->getSingleOccurrenceFPGANodes();
//         if(!nodes.empty()){
//             for(auto node:nodes){
//                 node->gain+=1;
//                 NodeSet neinodes = node->getneiNode();
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
    }
    //面积约束检查
    
    if (tarfpga->uesarea+node->area>tarfpga->area){
        return false;
    }
    return true;
}

void Point (Node* node){
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
                fpgas[nor_fpga]--;
                fpgas[tar_fpga]++;
                if (fpgas[nor_fpga] == 0) {
                    fpgas.erase(nor_fpga);
                }//更新移动后的边占fpga
                int points1=0;
                for (auto fpga2:fpgas){
                    int points1 += 空函数(tar_fpga.first,fpga2.first);   
                }//计算移动后分数
                gain[tar_fpga]+=points0-points1;
            };
        }//移动节点是源节点情况时，任何情况都要重新计算
        else{
            for (auto tar_fpga:tarfpgas){
                auto it = fpgas.find(tar_fpga);
                if(fpgas[nor_fpga]==1){//块和节点边交集大于1
                     gain[tar_fpga]=空函数(src_fpga,nor_fpga)-空函数(src_fpga,tar_fpga);
                }
                else if(it == fpgas.end()){//目标块和当前边无交集
                    gain[tar_fpga]=空函数(src_fpga,tar_fpga);
                }
                else{
                    gain[tar_fpga]=0;
                }
            }
        }
    }
    auto maxgain = std::max_element(gain.begin(), gain.end(),
    [](const std::pair<Fpga*, int>& a, const std::pair<Fpga*, int>& b) {
        return a.second > b.second;
    });
    node->gain=gain;
    node->maxgain = maxgain;
}

void delnode_GainFpgaMap(Node* node,GainFpgaMap &gainFpgamap){
    int gain = node->maxgain->second;
    auto it = gainFpgamap.find(gain);
    if (it != gainFpgamap.end()) {
        // 获取对应的节点列表
        auto &nodeList = it->second;

        // 从节点列表中删除该节点
        nodeList.erase(std::remove(nodeList.begin(), nodeList.end(), node), nodeList.end());

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

        // 在节点列表中移除当前节点
        nodeList.erase(std::remove(nodeList.begin(), nodeList.end(), node), nodeList.end());

        // 如果该增益值的节点列表为空，则移除该增益值
        if (nodeList.empty()) {
            gainFpgamap.erase(it);
        }
    }
    // 2. 更新节点的增益值
    int newGain=node->maxgain->second;

    // 3. 将节点插入新的增益值列表
    gainFpgamap[newGain].insert(node);

}

void updateNode(Node* node,Fpga* tarfpga){
    node->fpga=tarfpga;
}

void updateEdge(Hyperedge* edge,Fpga* oldfpga,Fpga* tarfpga){
        FpgaMap edgefpgas = edge->fpgaCount;
        edgefpgas[oldfpga]--;
        edgefpgas[tarfpga]++;
        if (edgefpgas[oldfpga] == 0) {
            edgefpgas.erase(oldfpga);
        }
}

void updateFpga(Node* node,Fpga* oldfpga,Fpga* tarfpga){
    oldfpga->nodes.erase(node);
    tarfpga->nodes.insert(node);
}

int Move (Node* node,GainFpgaMap &gainFpgamap){
    Fpga* oldfpga = node->fpga;
    Fpga* tarfpga=node->maxgain->first;
    NodeSet neinodes = node->getneiNode();
    for(auto edge:node->hyperedges){
        updateEdge(edge,oldfpga,tarfpga);
    }//更新移动后的边占fpga
    updateFpga(node,oldfpga,tarfpga);//更新fpga节点
    updateNode(node,tarfpga);
    node->movenable=false;
    delnode_GainFpgaMap(node,gainFpgamap);
    int maxpoints = 0;
    for(auto neinode:neinodes){
        int oldgain=neinode->maxgain->second;
        neinode->gain.clear();
        Point(neinode);
        maxpoints=neinode->maxgain->second>maxpoints?neinode->maxgain->second:maxpoints;
        update_GainFpgaMap(neinode,oldgain,gainFpgamap);
    }
    return maxpoints;
}




void addpoints (HyperGraph &HyperGraph){
    HyperedgeSet edges=HyperGraph.Edge_vector;

    for(auto edge:edges){
        Fpga* src_fpga=edge->src_node->fpga;//边的源节点
        FpgaMap fpgas=edge->fpgaCount;
        int points0=0;
        for (auto fpga:fpgas){
            int points0+=空函数(src_fpga,fpga.first);
        }
        edge->points=points0;
    }
}

void Partitioning(HyperGraph &HyperGraph,FpgaVector& fpgas,int max){
    addpoints (HyperGraph);
    NodeVector Nodes =HyperGraph.Node_vector;
    int maxgain=0;
    GainFpgaMap gainFpgamap;
    for (auto node:Nodes){
        Point(node);
        int gain=node->maxgain->second;
        gainFpgamap[gain].insert(node);
        maxgain=gain>maxgain?gain:maxgain;
    }    
    while (maxgain>0){
        Node* movenode=*(gainFpgamap[maxgain].begin());
        int newpoints=Move(movenode,gainFpgamap);
        maxgain=gainFpgamap.find(maxgain) != gainFpgamap.end()?maxgain:-1;

    }
    
    
    
}



