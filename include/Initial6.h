#include <cmath>
#include <numeric>



namespace Initial6{
    struct PartitionResult {
        HyperGraph* hypergraph;
        FpgaVector* fpgas;
        int points;
        bool success;
    };
    Fpga* getcenterFpga(const FpgaVector& Fpgas) {
        // 使用 std::max_element 查找综合距离最小的FPGA
        auto farthestFpga = std::min_element(Fpgas.begin(), Fpgas.end(), [](const Fpga* a, const Fpga* b) {
            // 计算每个FPGA的综合距离
            int sumA = std::accumulate(a->distance_neifpga.begin(), a->distance_neifpga.end(), 0);
            int sumB = std::accumulate(b->distance_neifpga.begin(), b->distance_neifpga.end(), 0);
            return sumA < sumB; 
        });

        return (farthestFpga != Fpgas.end()) ? *farthestFpga : nullptr;
    }

    // 将整数向量缩放到最大值
    void scaleIntegersToMax(std::vector<int>& numbers, int targetMax) {
        if (numbers.empty()) return;

        // 找到当前的最大值
        int currentMax = *std::max_element(numbers.begin(), numbers.end());

        // 计算比例因子
        double scaleFactor = static_cast<double>(targetMax) / currentMax;

        // 对每个元素进行等比放大
        for (int& num : numbers) {
            num = static_cast<int>(num * scaleFactor);
        }
    }

    int findClosestDistance(std::unordered_map<int, std::vector<Node*>>& re_distance, int target) {
        int offset = 0;
        while (true) {
            if (re_distance.find(target + offset) != re_distance.end() && !re_distance[target + offset].empty()) {
                return target + offset;
            }
            if (re_distance.find(target - offset) != re_distance.end() && !re_distance[target - offset].empty()) {
                return target - offset;
            }
            offset++;
        }
        return -1; // 理论上不会到达这里
    }
    // 按照BFS将未分配的节点分配给最近的已分配的FPGA
    void growNodes(std::vector<Fpga*>& Fpgas,ConstraintChecker &checker,std::mt19937 &engine) {
        std::vector<std::queue<Node*>> queues(Fpgas.size());

        // 将每个 FPGA 的已分配节点加入各自的队列
        for (size_t i = 0; i < Fpgas.size(); ++i) {
            for (auto& node : Fpgas[i]->nodes) {
                queues[i].push(node);
                for (Node* neighbor : node->getneiNode()) {
                    queues[i].push(neighbor);
                }
            }
        }
        int a=0;
        // 循环直至所有队列都为空
        bool allQueuesEmpty = false;
        while (!allQueuesEmpty) {
            allQueuesEmpty = true;


            std::shuffle(queues.begin(), queues.end(), engine);


            for (size_t i = 0; i < Fpgas.size(); ++i) {
                if (!queues[i].empty()) {
                    allQueuesEmpty = false;

                    
                    while (true){
                        Node* current = queues[i].front();
                        queues[i].pop();
                        if (checker.checkadd(current,Fpgas[i],checker)){
                                for (Node* neighbor : current->getneiNode()) {
                                    queues[i].push(neighbor);
                                }
                                Fpgas[i]->add_node(current);
                                break;
                        }
                        if(queues[i].empty()){
                            break;
                        }
                    }
                }

            }
        } 
    }

    void con_initial(HyperGraph &HyperGraph, FpgaVector &Fpgas,ConstraintChecker &checker){
        NodeVector &nodes=HyperGraph.Node_vector;
        for(Node* node : nodes){
            if(node->fpga == nullptr){
                
                for(auto fpga:node->getneifpga()){
                    if(checker.checkadd(node,fpga,checker)){ 
                        fpga->add_node(node);
                    }
                }

            }
        }
    }
    /**
     * @brief Performs initial partitioning of nodes in a hypergraph across multiple FPGAs.
     * 
     * This function partitions the nodes of a given hypergraph into multiple FPGAs using a breadth-first search (BFS) 
     * starting from the node with the smallest degree. The nodes are distributed to FPGAs based on their distance 
     * from the starting node and the scaled distances of the FPGAs.
     * 
     * @param HyperGraph The hypergraph containing the nodes to be partitioned.
     * @param Fpgas A vector of FPGAs to which the nodes will be assigned.
     * 
     * The partitioning process involves the following steps:
     * 1. Calculate the total number of nodes and the number of FPGAs.
     * 2. Determine the minimum size of nodes per FPGA and any extra elements.
     * 3. Select the node with the smallest degree as the starting node.
     * 4. Perform a BFS to determine the distance of each node from the starting node.
     * 5. Identify the FPGA that is farthest in terms of distance.
     * 6. Scale the distances of the FPGAs to the maximum distance found in the BFS.
     * 7. Assign nodes to FPGAs based on the closest distance match.
     */
    void InitialPartitioning(HyperGraph &HyperGraph, FpgaVector &Fpgas,ConstraintChecker &checker,unsigned int seed) {
        int totalNodes = HyperGraph._NumNode;
        int numFpgas = Fpgas.size();
        int minSize = totalNodes / numFpgas;
        int extraElements = totalNodes % numFpgas;
        int sqrtNumFpgas = std::sqrt(numFpgas);
        NodeVector& nodes = HyperGraph.Node_vector;
        HyperedgeSet &edges=HyperGraph.Edge_vector;
        //使用std::max_element找到包含节点最多的边

        auto maxEdge = std::max_element(edges.begin(), edges.end(), [](Hyperedge* a, Hyperedge* b) {
            return a->nodes.size() > b->nodes.size();
        });
        auto a=(*maxEdge);
        Node* startNode=(*maxEdge)->src_node.top();

        // BFS遍历
        std::queue<Node*> q;
        std::unordered_map<Node*, int> distance;//节点到起始节点的距离
        std::unordered_map<int, std::vector<Node*>> re_distance;
        q.push(startNode);
        distance[startNode] = 0;
        re_distance[0].push_back(startNode);

        //BFS遍历得到每个节点到起始节点的距离
        int visitedNodes = 1;//初始化计数
        int maxdis=0;
        while (visitedNodes < totalNodes) {
            Node* current = q.front();
            q.pop();
            for (Node* neighbor : current->getneiNode()) { // 使用 getneinode() 获取相邻节点
                if (distance.find(neighbor) == distance.end()) {
                    maxdis = distance[current] + 1;
                    distance[neighbor] = maxdis;
                    re_distance[maxdis].push_back(neighbor); // 将节点添加到对应距离的列表中
                    q.push(neighbor);
                    visitedNodes++; // 增加已访问节点计数
                    if (visitedNodes >= totalNodes) {
                        break; // 如果已访问节点数量达到最大节点数量，停止遍历
                    }
                }
            }
        }
        
        //std::cout << "Distance map contains " << distance.size() << " elements." << std::endl;
        auto farthestFpga = Initial6::getcenterFpga(Fpgas);//获得总距离最远的fpga作为初始fpga
        std::vector<int> fpgadis=farthestFpga->distance_neifpga;
        scaleIntegersToMax(fpgadis, maxdis);//将fpga的距离缩放到最大值
        int id = 0;
        std::mt19937 engine(seed);
        for(int dis: fpgadis){
            int distanceToUse = Initial6::findClosestDistance(re_distance, dis);
            
            std::uniform_int_distribution<int> dist(0, re_distance[distanceToUse].size() - 1);
            int randomNumber = dist(engine);
            Fpgas[id]->add_node(re_distance[distanceToUse][randomNumber]);//将距离最近的节点分配给fpga
            re_distance[distanceToUse].erase(re_distance[distanceToUse].begin() + randomNumber);
            id++;
        }
        Initial6::growNodes(Fpgas,checker,engine);
        con_initial(HyperGraph,Fpgas,checker);
    }

    std::vector<HyperGraph*> copygraphs(HyperGraph &Graph, int num) {
        std::vector<HyperGraph*> hypergraphs;
        for (int i = 0; i < num; ++i) {
            hypergraphs.push_back(new HyperGraph(Graph));
        }
        return hypergraphs;
    }

    std::vector<FpgaVector*> copyFpga(FpgaVector &fpgas, int num) {
        std::vector<FpgaVector*> fpgasvector;
        for (int i = 0; i < num; ++i) {
            FpgaVector* fpgasCopy = new FpgaVector();
            for (auto fpga : fpgas) {
                fpgasCopy->push_back(new Fpga(*fpga));
            }
            fpgasvector.push_back(fpgasCopy);
        }
        return fpgasvector;
    }
    std::mutex seed_mutex;
    PartitionResult run_initial_partitioning(HyperGraph* hypergraph, FpgaVector* fpgas, ConstraintChecker checker, unsigned int seed) {
        
        Initial6::InitialPartitioning(*hypergraph, *fpgas, checker, seed);
        int points = checker.check(*fpgas, *hypergraph);
        int hasinitial=0;
        std::lock_guard<std::mutex> lock(seed_mutex);
        for(auto fpga: *fpgas){
            fpga->print();
            hasinitial+=fpga->nodes.size();
        }        
        std::cout << "hasinitial:" << hasinitial << std::endl;
        bool success = hasinitial==hypergraph->_NumNode;
        return {hypergraph, fpgas, points, success};
    }
    PartitionResult multinitial(HyperGraph &hyperGraph, FpgaVector &Fpgas,ConstraintChecker &checker,unsigned int initial_seed,int num_seeds){
        std::mt19937 generator(initial_seed); // 使用初始种子初始化随机数生成器
        std::vector<unsigned int> seeds;
        // 生成多个种子
        for (int i = 0; i < num_seeds; ++i) {
            unsigned int new_seed = generator();
            seeds.push_back(new_seed);
            std::cout << "Seed " << i + 1 << ": " << new_seed << std::endl;
        }
        std::vector<HyperGraph*> hypergraphs = Initial6::copygraphs(hyperGraph, num_seeds);
        std::vector<FpgaVector*> fpgasvector = Initial6::copyFpga(Fpgas, num_seeds);
        std::vector<std::future<Initial6::PartitionResult>> futures;
        for (int i = 0; i < num_seeds; ++i) {
            futures.emplace_back(std::async(std::launch::async, run_initial_partitioning, hypergraphs[i], fpgasvector[i], checker, seeds[i]));
        }
        std::vector<PartitionResult> results;
        for (auto& future : futures) {
            results.push_back(future.get());
        }
        auto best_result_it = std::min_element(results.begin(), results.end(), [](const PartitionResult& a, const PartitionResult& b) {
            if (a.success && b.success) {
                return a.points < b.points; // 两者都成功时，比较 points
            } else if (a.success) {
                return true; // 只有 a 成功时，a 优先
            } else if (b.success) {
                return false; // 只有 b 成功时，b 优先
            } else {
                return a.points < b.points; // 两者都不成功时，仍然比较 points（尽管这种情况可能不常见）
            }
        });
        for(auto& result:results){
            if(&result != &(*best_result_it)){
                result.hypergraph->clear();
                for(auto fpga:*result.fpgas){
                    delete fpga;
                }
                delete result.hypergraph;
                delete result.fpgas;
            }
            else{
                int a=0;
            }
        }
        return *best_result_it;
    }
}