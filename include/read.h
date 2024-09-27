#include <fstream>
#include <sstream>
#include <regex> 
#include <queue>
// 读取节点文件
NodeVector readNodes(const std::string filename) {
    std::ifstream infile(filename);
    std::string line;
    NodeVector nodes;
    std::regex node_regex("g(\\d+)");
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string node_name;
        int area[8];
        iss >> node_name;
        std::smatch match;
        size_t id;
        std::regex_search(node_name, match, node_regex);
        id = std::stoi(match[1].str())-1; // 转换为整数
        for (int i = 0; i < 8; ++i) {
            iss >> area[i];
        }
        Node* node = new Node(area,id); 
        nodes.push_back(node);
    }
    return nodes;
}

// 读取超边文件
HyperedgeSet readEdges(const std::string filename, NodeVector& NodeVector) {
    std::ifstream infile(filename);
    std::string line;
    HyperedgeSet Hyperedges;
    std::regex node_regex("g(\\d+)");
    size_t id=0;
    while (std::getline(infile, line)) {
        Hyperedge* edge =new Hyperedge(id); 
        std::istringstream iss(line);
        std::string srnode_name;
        int area[8];
        iss >> srnode_name;
        std::smatch match;
        size_t node_id;
        std::regex_search(srnode_name, match, node_regex);
        node_id = std::stoi(match[1].str())-1; 
        edge->src_node=NodeVector[node_id];
        edge->addNode(NodeVector[node_id]);
        int weight;
        iss >> weight;
        edge->setWeight(weight);
        while (iss >> srnode_name)
        {
            std::regex_search(srnode_name, match, node_regex);
            node_id = std::stoi(match[1].str())-1; 
            edge->addNode(NodeVector[node_id]);
        }
        Hyperedges.insert(edge);
        id++;
    }
    return Hyperedges;
}

//读取FPGA文件
FpgaSet readFpgas(const std::string filename){
    std::ifstream infile(filename);
    std::string line;
    FpgaSet fpgas;
    std::regex fpga_regex("FPGA(\\d+)");
    size_t maxId = 0;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string fpga_name;
        int area[8];
        iss >> fpga_name;
        std::smatch match;
        size_t id;
        std::regex_search(fpga_name, match, fpga_regex);
        id = std::stoi(match[1].str())-1; // 转换为整数
        maxId = std::max(maxId, id + 1);
        for (int i = 0; i < 8; ++i) {
            iss >> area[i];
        }
        Fpga* fpga = new Fpga(area,id); 
        fpgas.push_back(fpga);
    }
    for (auto fpga : fpgas) {
        fpga->initial_Distance(maxId);
    }
    return fpgas;
}

void readtopo(const std::string filename, int hop_max,FpgaVector fpgas){
    std::ifstream infile(filename);
    std::string line;
    std::getline(infile, line);
    hop_max=std::stoi(line);
    std::regex fpga_regex("FPGA(\\d+)");
    std::vector<int>* adjList;
    adjList = new std::vector<int>[fpgas[0]->numFpgas];//创建邻接表
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string fpga_name_1;
        std::string fpga_name_2;
        std::smatch match_1;
        std::smatch match_2;
        int id[2];
        iss >> fpga_name_1;
        std::regex_search(fpga_name_1, match_1, fpga_regex);
        id[0] = std::stoi(match_1[1].str())-1; 
        iss >> fpga_name_2;
        std::regex_search(fpga_name_2, match_2, fpga_regex);
        id[1] = std::stoi(match_2[1].str())-1; 
        adjList[id[0]].push_back(id[1]);
        adjList[id[1]].push_back(id[0]);
    };
    for (int i = 0; i < fpgas[0]->numFpgas; ++i){
        BFS(i,adjList,fpgas);
    }
}

void BFS(int startVertex,std::vector<int>* adjList,FpgaVector fpgas) {
        std::vector<bool> visited(fpgas[0]->numFpgas, false);   // 标记节点是否被访问
        std::queue<int> queue;                           // 用于BFS的队列
        std::queue<int> tem_queue;
 
        // 将起始节点入队并标记为已访问
        queue.push(startVertex);
        visited[startVertex] = true;
 
        while (!queue.empty()) {
            int currentVertex = queue.front();
            //bool onlyOneElement = queue.size() == 1;//避免queue.size()变化影响判断
            queue.pop();
            for (int neighbor : adjList[currentVertex]) {
                if (!visited[neighbor]) {
                    tem_queue.push(neighbor);
                }
            }
            if (queue.empty()) {
                for (int i = 0; i < fpgas[0]->numFpgas; ++i) {
                    if (!visited[i]) {
                        fpgas[startVertex]->distance_neifpga[i] ++;
                }
            }
               while (!tem_queue.empty()) {
                int nextVertex = tem_queue.front();
                tem_queue.pop();
                queue.push(nextVertex);
                visited[nextVertex] = true;
        }
    }}}