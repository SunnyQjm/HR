//
// Created by sunny on 2020/2/24.
//

#include "LibXLHelper.h"
#include <utility>

LibXLHelper::LibXLHelper(string fileName, Type type) : fileName(std::move(fileName)) {
#ifndef _WIN32
    book = xlCreateBook();
    sheet = book->addSheet("Sheet1");
    curRow = 1;
    switch (type) {
        case ROUTE_INFO:
            sheet->writeStr(curRow, 0, "src");
            sheet->writeStr(curRow, 1, "des");
            sheet->writeStr(curRow, 2, "result");
            sheet->writeStr(curRow, 3, "path");
            sheet->writeStr(curRow, 4, "total distance");
            sheet->writeStr(curRow, 5, "hop num");
            break;
        case DEGREE_INFO:
            sheet->writeStr(curRow, 0, "nodeId");
            sheet->writeStr(curRow, 1, "degree");
            break;
    }
#endif
}

LibXLHelper::~LibXLHelper() {
#ifndef _WIN32

    book->save(this->fileName.c_str());
#endif
}

const LibXLHelper &LibXLHelper::addItem(const LibXLHelper::Item &item) {
#ifndef _WIN32
    curRow++;
    sheet->writeNum(curRow, 0, item.src);
    sheet->writeNum(curRow, 1, item.des);
    sheet->writeStr(curRow, 2, item.result.c_str());
    sheet->writeStr(curRow, 3, item.path.c_str());
    sheet->writeNum(curRow, 4, item.totalDistance);
    sheet->writeNum(curRow, 5, item.hopNum);
#endif
    return *this;
}

const LibXLHelper &LibXLHelper::addDegreeInfoItem(const LibXLHelper::DegreeInfoItem &degreeInfoItem) {
#ifndef _WIN32
    curRow++;
    sheet->writeNum(curRow, 0, degreeInfoItem.nodeId);
    sheet->writeNum(curRow, 1, degreeInfoItem.degree);
#endif
    return *this;
}
