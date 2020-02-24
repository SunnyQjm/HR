//
// Created by sunny on 2020/2/24.
//

#include "LibXLHelper.h"
#include <utility>

LibXLHelper::LibXLHelper(string fileName) : fileName(std::move(fileName)) {
    book = xlCreateBook();
    sheet = book->addSheet("Sheet1");
    curRow = 1;
    sheet->writeStr(curRow, 0, "src");
    sheet->writeStr(curRow, 1, "des");
    sheet->writeStr(curRow, 2, "result");
    sheet->writeStr(curRow, 3, "path");
    sheet->writeStr(curRow, 4, "total distance");
    sheet->writeStr(curRow, 5, "hop num");
}

LibXLHelper::~LibXLHelper() {
    book->save(this->fileName.c_str());
}

const LibXLHelper &LibXLHelper::addItem(const LibXLHelper::Item &item) {
    curRow++;
    sheet->writeNum(curRow, 0, item.src);
    sheet->writeNum(curRow, 1, item.des);
    sheet->writeStr(curRow, 2, item.result.c_str());
    sheet->writeStr(curRow, 3, item.path.c_str());
    sheet->writeNum(curRow, 4, item.totalDistance);
    sheet->writeNum(curRow, 5, item.hopNum);
    return *this;
}
