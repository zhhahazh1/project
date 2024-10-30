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
