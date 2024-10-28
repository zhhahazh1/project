
bool checkcon(Node* node,Fpga* tarfpga,ConstraintChecker &checker){
    HyperedgeSet neiedges = node->hyperedges_less;
    Fpga* nor_fpga=node->fpga;//移动前位置
    //边约束检查
    for (auto edge:neiedges){
        if(node==edge->src_node.top()){
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
            int distance =dis_fpgas(tarfpga,edge->src_node.top()->fpga);
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
        fpgacount[nor_fpga]-=1;
        if(fpgacount[nor_fpga]==0){
            fpgacount.erase(nor_fpga);
        }
        if(fpgacount[tarfpga]==1){
            tarfpga_nowcop+=edge->weight;
        }
        if(fpgacount.size()==1){
            tarfpga_nowcop-=edge->weight;
        }
    }
    if (tarfpga_nowcop>tarfpga->maxcoppoints){
            return false;
    }
    // //检查当前fpga的cop
    // int norfpga_nowcop=node->fpga->nowcoppoints;
    // for (auto edge:neiedges){
    //     FpgaMap fpgacount=edge->fpgaCount;//复制一份fpgacount
    //     if(fpgacount.size()==1){
    //         norfpga_nowcop+=edge->weight;
    //         if (norfpga_nowcop>node->fpga->maxcoppoints){
    //             return false;
    //         }
    //     }
        
    // }
    return true;
}

int onepoint(Node* node,Hyperedge* edge,Fpga* tar_fpga){
//确定可以移动再调用这个函数
    int gain=0;
    FpgaMap fpgas=edge->fpgaCount;//这条边所包含的fpga
    Fpga* nor_fpga=node->fpga;//移动前位置的fpga
    int points0=edge->points;//边的初始分数
    if (node==edge->src_node.top()){
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
        Fpga* src_fpga=edge->src_node.top()->fpga;//边的源节点
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
    HyperedgeSet neiedges = node->hyperedges_less;
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
    
    HyperedgeSet moved_node_neiedges = moved_node->hyperedges_less;
    HyperedgeSet repoint_node_neiedges = repoint_node->hyperedges_less;
    FpgaSet tar_fpgas=repoint_node->getneifpga();//移动目标范围
    Fpga* nor_fpga=repoint_node->fpga;//repoint_node移动前位置
    FpgaMap new_gain;
    FpgaMap oldgain=repoint_node->gain;
    for (auto tar_fpga:tar_fpgas){
        //对于原本就没有的tarfpga，要从头计算
        if(oldgain.find(tar_fpga)==oldgain.end()){
            for(auto edge:repoint_node->hyperedges_less){
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
    Fpga* src_fpga=edge->src_node.top()->fpga;//边的源节点
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

// 包装函数，用于处理一部分 neinode
void process_neinode_range(std::set<Node*>::iterator start, std::set<Node*>::iterator end, Node* node, GainFpgaMap& gainFpgamap) {
    for (auto it = start; it != end; ++it) {
        process_neinode(*it, node, gainFpgamap);
    }
}
void Move (NodeMove move,GainFpgaMap &gainFpgamap,ConstraintChecker &checker){
    Node* node=move.first;
    Fpga* tarfpga=move.second;
    Fpga* oldfpga = node->fpga;
    NodeSet neinodes = node->getneiNode();
    oldfpga->erase_node(node);
    tarfpga->add_node(node);
    for(auto edge:node->hyperedges_less){
        updateEdge_points(edge);
    }
    node->movenable=false;
    delnode_GainFpgaMap(node,node->gain,gainFpgamap);
    int maxpoints = 0;

    START_TIMING();  // 开始计时
    std::vector<std::thread> threads;
    // 获取系统的核心数
    unsigned int num_cores = std::thread::hardware_concurrency();
    // 计算每个线程处理的 neinode 数量
    size_t num_neinodes = neinodes.size();
    // 限制线程数不能超过节点数
    unsigned int actual_threads = std::min(num_cores, static_cast<unsigned int>(num_neinodes));
    size_t chunk_size = (num_neinodes + actual_threads - 1) / actual_threads; // 向上取整
    // 为每个核心创建一个线程
    for (unsigned int i = 0; i < actual_threads; ++i) {
        auto start = std::next(neinodes.begin(), i * chunk_size);
            // 检查剩余元素是否足够分配给当前线程
        if (std::distance(start, neinodes.end()) <= chunk_size) {
            // 如果不足，将剩余的所有元素分配给该线程
            threads.emplace_back(std::thread(process_neinode_range, start, neinodes.end(), node, std::ref(gainFpgamap)));
            break; // 停止创建更多线程
        } else {
            // 正常分配区块
            auto end = std::next(start, chunk_size);
            threads.emplace_back(std::thread(process_neinode_range, start, end, node, std::ref(gainFpgamap)));
        }
    }
    // 等待所有线程执行完毕
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    END_TIMING(stats.totalDuration, stats.callCount);  // 结束计时并记录
    stats.printStatistics();
}




void addpoints (HyperGraph &HyperGraph){
    HyperedgeSet &edges=HyperGraph.Edge_vector;

    for(auto edge:edges){
        Fpga* src_fpga=edge->src_node.top()->fpga;//边的源节点
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
                    bool a=checker.checkmaxcop(fpgas,HyperGraph);
                    Move(movenode,gainFpgamap,checker);
                    a=checker.checkmaxcop(fpgas,HyperGraph);
                    hasmove=true;
                    
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



