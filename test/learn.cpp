#include <iostream>

int main() {
    int day=2;
    

    switch (day) {
        case 1:
            std::cout << "星期一" << std::endl;
        case 2:
            std::cout << "星期二" << std::endl;
        case 3:
            std::cout << "星期三" << std::endl;
            break;
        case 4:
            std::cout << "星期四" << std::endl;
        case 5:
            std::cout << "星期五" << std::endl;
            break;
        case 6:
            std::cout << "星期六" << std::endl;
            break;
        case 7:
            std::cout << "星期日" << std::endl;
            break;
        default:
            std::cout << "eee" << std::endl;
            break;
    }

    return 0;
}
