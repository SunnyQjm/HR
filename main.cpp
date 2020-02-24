#include <iostream>
#include "HR_graph.hpp"
#include <ctime>
#include "LibXLHelper.h"

int main()
{
    srand(245);
    RandomGraph g;
//	g.print();
    //system("pause");
    int successNum = 0;
    int totalNum = 10000;
    LibXLHelper libXlHelper;
    srand((unsigned)time(NULL));
    for (int i = 0; i < totalNum; i++) {
        if(g.randomRoutingTest(&libXlHelper)) {
            successNum++;
        }
    }
    cout << "路由成功率：" << successNum * 100.0 / totalNum << "%" << endl;
}