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
HyperedgeVector readEdges(const std::string filename, NodeVector& NodeVector) {
    std::ifstream infile(filename);
    std::string line;
    HyperedgeVector Hyperedges;
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
        Hyperedges.push_back(edge);
        id++;
    }
    return Hyperedges;
}

// FpgaVector readfpga(const std::string filename){
// }

// void readtopo(const std::string filename,FpgaVector &fpgas){
    
// }
// void add_link(Fpga* fpga1,Fpga* fpga2){
//     fpga1->neifpga.push_back(fpga2);
//     fpga2->neifpga.push_back(fpga1);
// }
// void addtopf(FpgaVector &fpgas){
//     for(auto fpga:fpgas){
//         int* jvli;
//         for(auto neifpga:fpga->neifpga)
//             int id =neifpga->ID;
//             fpga->jvli[id]=1;
//             for
//     }
// }