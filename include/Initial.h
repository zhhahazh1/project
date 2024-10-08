#include <cmath>
#include <numeric>
// void InitialPartitioning(HyperGraph &HyperGraph,FpgaSet &Fpgas){
//     int totalNodes = HyperGraph._NumNode;
//     int numFpgas = Fpgas.size();
//     int minSize = totalNodes / numFpgas;
//     int extraElements = totalNodes % numFpgas;
//     int sqrtNumFpgas = std::sqrt(numFpgas);
//     NodeSet& nodes=HyperGraph.Node_vector;
//     int start = 0;
//     int ID[2]={0,0};
//     for (int i = 0; i < numFpgas-1; ++i) {
//         int end = start + minSize;
//         ID[0]= i / sqrtNumFpgas;
//         ID[1]= i % sqrtNumFpgas;
//         Fpga* fpga=new Fpga(ID);
//         for (auto it = nodes.begin() + start; it != nodes.begin() + end; ++it) {
//             fpga->add_node(*it);
//             (*it)->fpga = fpga;
//             for(auto edge:(*it)->hyperedges){
//                 edge->fpgaCount[fpga]+=1;
//             }
//         }
//         (Fpgas)[i] = fpga;
//         start = end;
//     }
//     int end = start + minSize+extraElements;
//     ID[0] = sqrtNumFpgas - 1; 
//     ID[1] = sqrtNumFpgas - 1; 
//     Fpga* fpga=new Fpga(ID);
//     for (auto it = nodes.begin() + start; it != nodes.begin() + end; ++it) {
//         fpga->add_node(*it);
//         (*it)->fpga = fpga;
//         for(auto edge:(*it)->hyperedges){
//             edge->fpgaCount[fpga]+=1;
//         }
//     }
//     (Fpgas)[numFpgas-1] = fpga;
// }

Fpga* getFarthestFpga(const FpgaVector& Fpgas) {
    // 使用 std::max_element 查找综合距离最大的FPGA
    auto farthestFpga = std::max_element(Fpgas.begin(), Fpgas.end(), [](const Fpga* a, const Fpga* b) {
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

int findClosestDistance(std::unordered_map<int, std::queue<Node*>>& re_distance, int target) {
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
void growNodes(std::vector<Fpga*>& Fpgas) {
    std::vector<std::queue<Node*>> queues(Fpgas.size());

    // 将每个 FPGA 的已分配节点加入各自的队列
    for (size_t i = 0; i < Fpgas.size(); ++i) {
        for (auto& node : Fpgas[i]->nodes) {
            queues[i].push(node);
        }
    }
    int a=0;
    // 循环直至所有队列都为空
    bool allQueuesEmpty = false;
    while (!allQueuesEmpty) {
        allQueuesEmpty = true;
        for (size_t i = 0; i < Fpgas.size(); ++i) {
            if (!queues[i].empty()) {
                allQueuesEmpty = false;

                Node* current = queues[i].front();
                queues[i].pop();
                for (Node* neighbor : current->getneiNode()) {
                    queues[i].push(neighbor);
                }
                while (true){
                    auto currentnode = queues[i].front();
                    if ((currentnode->fpga == nullptr)&&(Fpgas[i]->usearea + currentnode->area < Fpgas[i]->area)){
                            Fpgas[i]->add_node(currentnode);
                            break;
                    }
                    queues[i].pop();
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
void InitialPartitioning(HyperGraph &HyperGraph, FpgaVector &Fpgas) {
    int totalNodes = HyperGraph._NumNode;
    int numFpgas = Fpgas.size();
    int minSize = totalNodes / numFpgas;
    int extraElements = totalNodes % numFpgas;
    int sqrtNumFpgas = std::sqrt(numFpgas);
    NodeVector& nodes = HyperGraph.Node_vector;

    // 选择度数最小的节点作为出发节点
    auto startNode = std::min_element(nodes.begin(), nodes.end(), [](Node* a, Node* b) {
        return a->getneiNode().size() < b->getneiNode().size();
    });

    // BFS遍历
    std::queue<Node*> q;
    std::unordered_map<Node*, int> distance;//节点到起始节点的距离
    std::unordered_map<int, std::queue<Node*>> re_distance;
    q.push(*startNode);
    distance[*startNode] = 0;
    re_distance[0].push(*startNode);

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
                re_distance[maxdis].push(neighbor); // 将节点添加到对应距离的列表中
                q.push(neighbor);
                visitedNodes++; // 增加已访问节点计数
                if (visitedNodes >= totalNodes) {
                    break; // 如果已访问节点数量达到最大节点数量，停止遍历
                }
            }
        }
    }
    //std::cout << "Distance map contains " << distance.size() << " elements." << std::endl;
    auto farthestFpga = getFarthestFpga(Fpgas);//获得总距离最远的fpga作为初始fpga
    std::vector<int> fpgadis=farthestFpga->distance_neifpga;
    scaleIntegersToMax(fpgadis, maxdis);//将fpga的距离缩放到最大值
    int id = 0;
    for(int dis: fpgadis){
        int distanceToUse = findClosestDistance(re_distance, dis);
        Fpgas[id]->add_node(re_distance[distanceToUse].front());//将距离最近的节点分配给fpga
        re_distance[distanceToUse].front()->fpga = Fpgas[id];//更新节点的fpga
        re_distance[distanceToUse].pop();
        id++;
    }
    growNodes(Fpgas);
}