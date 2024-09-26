#include <fstream>
#include <sstream>
#include <regex> 
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
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string fpga_name;
        int area[8];
        iss >> fpga_name;
        std::smatch match;
        size_t id;
        std::regex_search(fpga_name, match, fpga_regex);
        id = std::stoi(match[1].str())-1; // 转换为整数
        for (int i = 0; i < 8; ++i) {
            iss >> area[i];
        }
        Fpga* fpga = new Fpga(area,id); 
        fpgas.push_back(fpga);
    }
    return fpgas;
}

FpgaVector readtopo(const std::string filename, int fpga_max_dis){
    std::ifstream infile(filename);
    std::string line;
    std::getline(infile, line);
    fpga_max_dis=std::stoi(line);
    std::regex fpga_regex("FPGA(\\d+)");
    FpgaVector fpgas;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string fpga_name;
        std::smatch match;
        int id[2];
        for (int i = 0; i < 2; ++i) {
        iss >> fpga_name;
        std::regex_search(fpga_name, match, fpga_regex);
        id[0] = std::stoi(match[1].str())-1; 
        }
        fpgas[id[0]]->distance_neifpga[id[1]] = 1;
        fpgas[id[1]]->distance_neifpga[id[0]] = 1;
    }
    return fpgas;
}