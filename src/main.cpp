#include <class.h>
#include <read.h>
int main() {
    std::string desing_aera = "../exapmle/desing.area";
    std::string desing_net = "../exapmle/desing.net";
    NodeVector nodes = readNodes(desing_aera);

    HyperedgeVector Hyperedge =readEdges(desing_net, nodes);



    return 0;
}
