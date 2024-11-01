#include <iostream>
#include <fstream>  // 包含文件流库


int writhe(FpgaVector &fpgas, std::string& outputFile) {
    // 创建一个输出文件流对象，指定输出文件名
    std::ofstream outfile(outputFile);

    for(auto fpga: fpgas){
        outfile << "FPGA" << fpga->ID+1 << ": ";
        for(auto node:fpga->nodes){
            outfile <<"g"<< node->ID+1 << " ";
        }
        outfile << std::endl;
    }

    // 关闭文件流
    outfile.close();

    std::cout << "数据已成功写入到 output.txt 文件。" << std::endl;

    return 0;
}
void write2(HyperGraph & g, std::string& outputFile,std::string& outputFile2){
    std::ofstream outfile(outputFile);
    for(auto node: g.Node_vector){
        outfile << "g" << node->ID+1 << " ";
        for(int i=0;i<8;i++){
        outfile << node->area.values[i] << " ";
        }
        outfile << std::endl;
    }
    outfile.close();
    std::ofstream outfile2(outputFile2);
    for(auto edge: g.Edge_vector){
        outfile2 << "g" << edge->src_node.top()->ID+1 << " ";
        outfile2 << edge->weight << " ";
        for(auto node: edge->nodes){
            if(node!=edge->src_node.top()){
                outfile2 << "g" << node->ID+1 << " ";
            }
        }
        outfile2 << std::endl;
    }
    outfile2.close();
}
