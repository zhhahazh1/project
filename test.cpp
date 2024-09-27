#include <fstream>
#include <sstream>
#include <regex> 
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>
#include <queue>

/*测试读入
int main() {
    std::string design_info = "/home/zwl/project/exapmle/design.are";
    std::ifstream infile(design_info);
    std::string line;
    std::regex fpga_regex("g(\\d+)");
    int a;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string fpga_name;
        int area[8];
        iss >> fpga_name;
        std::smatch match;
        size_t id;
        std::regex_search(fpga_name, match, fpga_regex);
        id = std::stoi(match[1].str())-1; // 转换为整数，match[1] 返回第一个括号内的匹配子串。
        for (int i = 0; i < 8; ++i) {
            iss >> area[i];
        }
    }
    return 0;
}*/

//测试读取topo
class Fpga;
class Fpga {
public:
    Fpga(const int *area,size_t id) {  
        for (int i = 0; i < 8; ++i) {
            this->area[i] = area[i];
        }
        this->ID=id;
        //this->distance_neifpga = nullptr;
    }
    void initial_Distance(size_t numFpga) {
        this->numFpgas=numFpga;
        this->distance_neifpga.resize(numFpgas, 0);
        adjList = new std::vector<int>[numFpgas];
    }
    int area[8]; 
    size_t ID;
    std::vector<int> distance_neifpga;
    std::vector<int>* adjList;
    size_t numFpgas;
};

using FpgaVector = std::vector<Fpga*>;
FpgaVector readFpgas(const std::string filename){
    std::ifstream infile(filename);
    std::string line;
    FpgaVector fpgas;
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

void readtopo(const std::string filename, int fpga_max_dis){
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

int main() {
    std::string design_info = "/home/zwl/project/exapmle/design.info";
    FpgaVector fpgas = readFpgas(design_info);
    std::string design_topo = "/home/zwl/project/exapmle/design.topo";
    int hop_max;
    std::ifstream infile(design_topo);
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
        //fpgas[id[0]]->distance_neifpga[id[1]] = 1;
        //fpgas[id[1]]->distance_neifpga[id[0]] = 1;
        adjList[id[0]].push_back(id[1]);
        adjList[id[1]].push_back(id[0]);
    };
    for (int i = 0; i < fpgas[0]->numFpgas; ++i){
        BFS(i,adjList,fpgas);
    }
    return 0;
}