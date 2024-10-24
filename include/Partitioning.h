
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

int onepoint(Node* node,Hyperedge* edge,Fpga* tar_fpga){
//确定可以移动再调用这个函数
    int gain=0;
    FpgaMap fpgas=edge->fpgaCount;//这条边所包含的fpga
    Fpga* nor_fpga=node->fpga;//移动前位置的fpga
    int points0=edge->points;//边的初始分数
    if (node==edge->src_node){
        fpgas[nor_fpga]--;
        fpgas[tar_fpga]++;
        if (fpgas[nor_fpga] == 0) {
            fpgas.erase(nor_fpga);
        }//更新移动后的边占fpga
        int points1=0;
        for (auto fpga2:fpgas){
            points1 += dis_fpgas(tar_fpga,fpga2.first);   
        }//计算移动后分数
        gain=points0-points1;              
    }
    else{
        Fpga* src_fpga=edge->src_node->fpga;//边的源节点
        auto it = fpgas.find(tar_fpga);
        if(fpgas[nor_fpga]==1){//如果这条边只有这个节点在这个foga中，则会加分
            gain=+dis_fpgas(src_fpga,nor_fpga);
        }
        if(it == fpgas.end()){//目标块和当前边无交集
            gain=-dis_fpgas(src_fpga,tar_fpga);
        }
    }
    return gain;
}
void Point (Node* node,ConstraintChecker &checker){
    node->gain.clear();
    HyperedgeSet neiedges = node->hyperedges;
    Fpga* nor_fpga=node->fpga;//移动前位置
    FpgaMap gains;//最后得到的东西
    auto &gain_edge=node->gain_edge;
    FpgaSet tarfpgas=node->getneifpga();//移动目标范围
    for (auto edge:neiedges){
        for (auto tarfpga:tarfpgas){
                int gain=onepoint(node,edge,tarfpga);
                gains[tarfpga]+=gain;
        }
    }
    node->gain=gains;
}


void Repoints(Node* repoint_node,const Node* moved_node){
    
    HyperedgeSet moved_node_neiedges = moved_node->hyperedges;
    HyperedgeSet repoint_node_neiedges = repoint_node->hyperedges;
    FpgaSet tar_fpgas=repoint_node->getneifpga();//移动目标范围
    Fpga* nor_fpga=repoint_node->fpga;//repoint_node移动前位置
    FpgaMap new_gain;
    FpgaMap oldgain=repoint_node->gain;
    for (auto tar_fpga:tar_fpgas){
        //对于原本就没有的tarfpga，要从头计算
        if(oldgain.find(tar_fpga)==oldgain.end()){
            for(auto edge:repoint_node->hyperedges){
                int gain=onepoint(repoint_node,edge,tar_fpga);
                new_gain[tar_fpga]=gain;
            }
        }
        //对于原本就有的tarfpga，只需要更新变化了的边
        else{
            new_gain[tar_fpga]=oldgain[tar_fpga];
            int gain=0;
            for(auto edge:moved_node_neiedges){
                if(repoint_node_neiedges.find(edge)!=repoint_node_neiedges.end()){
                    int newpoint=onepoint(repoint_node,edge,tar_fpga);
                    int oldpoint=repoint_node->gain_edge[tar_fpga][edge];
                    gain+=newpoint-oldpoint;
                    repoint_node->gain_edge[tar_fpga][edge]=newpoint;
                }
            }
            new_gain[tar_fpga]+=gain;
        }
    }
    repoint_node->gain=new_gain;
    
    
}

void delnode_GainFpgaMap(Node* node,FpgaMap oldgain,GainFpgaMap &gainFpgamap){
    for(auto gain:oldgain){
        gainFpgamap[gain.second].erase(std::make_pair(node,gain.first));
        if(gainFpgamap[gain.second].empty())
            gainFpgamap.erase(gain.second);
    }
}
void addnode_GainFpgaMap(Node* node,FpgaMap oldgain,GainFpgaMap &gainFpgamap){
    for(auto gain:oldgain){
        gainFpgamap[gain.second].insert(std::make_pair(node,gain.first));
    }
}

void update_GainFpgaMap (Node* node,FpgaMap oldgain, GainFpgaMap &gainFpgamap){
    // 1. 删除节点的旧增益值
    delnode_GainFpgaMap(node, oldgain, gainFpgamap);
    // 2. 更新节点的增益值
    FpgaMap newGain = node->gain;
    addnode_GainFpgaMap(node, newGain, gainFpgamap);
}




void updateEdge_points(Hyperedge* edge){
    Fpga* src_fpga=edge->src_node->fpga;//边的源节点
    FpgaMap fpgas=edge->fpgaCount;
    int points0=0;
    for (auto fpga:fpgas){
        points0+=edge->weight*dis_fpgas(src_fpga,fpga.first);
    }
    edge->points=points0;
}



std::mutex gainFpgamap_mutex;

void process_neinode(Node* neinode, Node* node, GainFpgaMap& gainFpgamap) {
    if (neinode->movenable != false) {
        FpgaMap oldgain = neinode->gain;
        Repoints(neinode, node);

        // 加锁保护 gainFpgamap，防止数据竞争
        {
            std::lock_guard<std::mutex> lock(gainFpgamap_mutex);
            update_GainFpgaMap(neinode, oldgain, gainFpgamap);  // 线程安全的修改
        }  // 这里锁会自动释放
    }
}

void Move (NodeMove move,GainFpgaMap &gainFpgamap,ConstraintChecker &checker){
    Node* node=move.first;
    Fpga* tarfpga=move.second;
    Fpga* oldfpga = node->fpga;
    NodeSet neinodes = node->getneiNode();
    oldfpga->erase_node(node);
    tarfpga->add_node(node);
    for(auto edge:node->hyperedges){
        updateEdge_points(edge);
    }
    node->movenable=false;
    delnode_GainFpgaMap(node,node->gain,gainFpgamap);
    int maxpoints = 0;
    std::vector<std::thread> threads;
    
    // 为每个 neinode 创建一个线程
    for (auto neinode : neinodes) {
        threads.emplace_back(std::thread(process_neinode, neinode, node, std::ref(gainFpgamap)));
    }
    
    // 等待所有线程执行完毕
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
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
    
    GainFpgaMap gainFpgamap;
    for (auto node:Nodes){
        Point(node,checker);
        for(auto gain:node->gain){
            gainFpgamap[gain.second].insert(std::make_pair(node,gain.first));
        }
    }
    int maxgain=gainFpgamap.rbegin()->first;
    while(maxgain>0){
        bool hasmove=false;
        
        while (maxgain>0){        
            for(auto movenode:gainFpgamap[maxgain]){
                if(checkcon(movenode.first,movenode.second,checker)){
                    START_TIMING();  // 开始计时
                    Move(movenode,gainFpgamap,checker);
                    hasmove=true;
                    END_TIMING(stats.totalDuration, stats.callCount);  // 结束计时并记录
                    stats.printStatistics();
                    break;
                }
            }
            if(hasmove){
                maxgain=gainFpgamap.rbegin()->first;
                break; 
            }
            maxgain--;
        }
    }
    int a=0;
}



