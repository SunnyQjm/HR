//
// Created by sunny on 2020/2/24.
//

#ifndef HR_LIBXLHELPER_H
#define HR_LIBXLHELPER_H
#include <libxl.h>
#include <string>

using namespace libxl;
using namespace std;

class LibXLHelper {
public:
    struct Item {
        int src;                    // 起点
        int des;                    // 终点
        string result;              // 路由结果
        string path;                // 路由路径
        int totalDistance = -1;          // 总距离
        int hopNum = -1;                 // 路由成功的跳数
    };
private:
    Book *book;
    Sheet* sheet;
    string fileName;
    int curRow = 0;

public:
    explicit LibXLHelper(string fileName = "test.xls");
    const LibXLHelper& addItem(const Item& item);
    ~LibXLHelper();
};


#endif //HR_LIBXLHELPER_H
