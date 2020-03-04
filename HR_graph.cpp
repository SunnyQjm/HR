#include <cstdlib>
#include <iostream>
#include "HR_graph.hpp"
#include "LibXLHelper.h"
#include <map>
#include <stack>
#include <set>

RandomGraph::RandomGraph() {
    //节点数目 = 地面节点 + 各高度的卫星节点
    nodeNum = BASE * (SCARE_FREE_NETWORK_SIZE + LOW_ORBIT_SIZE + MIDDLE_ORBIT_SIZE + HIGH_ORBIT_SIZE);
    nodeList = new Node *[nodeNum];
    adjacencyTable = new ChainListNode *[nodeNum];
    //随机生成点集
    int i = 0;
    for (; i < BASE * SCARE_FREE_NETWORK_SIZE; i++) {
        nodeList[i] = &NodeGenerator::randomSurficialNode();
        adjacencyTable[i] = NULL;
    }
    for (; i < BASE * (SCARE_FREE_NETWORK_SIZE + LOW_ORBIT_SIZE); i++) {
        nodeList[i] = &NodeGenerator::randomLowOrbitNode();
        adjacencyTable[i] = NULL;
    }
    for (; i < BASE * (SCARE_FREE_NETWORK_SIZE + LOW_ORBIT_SIZE + MIDDLE_ORBIT_SIZE); i++) {
        nodeList[i] = &NodeGenerator::randomMiddleOrbitNode();
        adjacencyTable[i] = NULL;
    }
    for (; i < nodeNum; i++) {
        nodeList[i] = &NodeGenerator::randomHighOrbitNode();
        adjacencyTable[i] = NULL;
    }
    long edge_num = 0;
    int zeroNum = 0;
    //计算邻接表
    ChainListNode *n;
    LibXLHelper libXlHelper("degree_info.xls", LibXLHelper::Type::DEGREE_INFO);
    LibXLHelper::DegreeInfoItem item{};
    for (int i = 0; i < nodeNum; i++) {
        int _edgeNum = 0;
        item.nodeId = i;
        for (int j = i + 1; j < nodeNum; j++) {
            if (nodeList[i]->checkIfConnectTo(*nodeList[j])) {
                //i,j相连
                n = new ChainListNode(j, adjacencyTable[i]);
                adjacencyTable[i] = n;
                n = new ChainListNode(i, adjacencyTable[j]);
                adjacencyTable[j] = n;
                edge_num += 2;
                _edgeNum += 2;
            }
        }
        item.degree = _edgeNum;
        libXlHelper.addDegreeInfoItem(item);
        if (_edgeNum == 0) {
            zeroNum++;
        }
//        hr_log("edge num: %d\n", _edgeNum);
    }
    hr_log("%d node have no adjacency\n", zeroNum);
    hr_log("node num: %d\n", nodeNum);
    hr_log("edge num: %ld\n", edge_num);
    hr_log("average degree is %ld\n", edge_num / nodeNum);
}

RandomGraph::~RandomGraph() {
    for (int i = 0; i < nodeNum; i++) {
        delete nodeList[i];
        ChainListNode *t = adjacencyTable[i];
        while (adjacencyTable[i] != NULL) {
            adjacencyTable[i] = adjacencyTable[i]->next;
            delete t;
            t = adjacencyTable[i];
        }
    }
    delete[] nodeList;
    delete[] adjacencyTable;
}

int RandomGraph::selectNeighbour(int curID, int desID, int i, std::map<int, int> map) const {
    double maxDis = BIG_ENOUGH;
    int next = -1;
    hr_debug("select next for node %d:\n", curID);
    ChainListNode *t = adjacencyTable[curID];
//    cout << "{ " << endl;
    while (t != NULL) {
//        cout << "(" << t->neighbourID << ", " << map[t->neighbourID] << "), ";
        if (map[t->neighbourID] != 0) {
            t = t->next;
            continue;
        }
        double dis = nodeList[t->neighbourID]->HyperbolicDistanceTo(*nodeList[desID]);
        hr_debug("  to node %d: %.3f\n", t->neighbourID, dis);
        if (dis < maxDis) {
            maxDis = dis;
            next = t->neighbourID;
        }
        t = t->next;
    }
//	cout << endl;
    return next;
}

bool RandomGraph::routingTest(int srcID, int desID, LibXLHelper *pHelper) const {
    LibXLHelper::Item item;
    item.src = srcID;
    item.des = desID;
    hr_log("src:%d, des:%d\n", srcID, desID);
    //记录访问数据
//    int *visited = new int[nodeNum];
    std::map<int, int> visited;
    //路由统计
    double totalDis = 0; //路由路径的总地理距离
    int hopNum = 0; //路由路径的跳数

    stack<int> pathStack;
    int curID = srcID;
    hr_log("%d(sur)", curID);
    item.path = to_string(curID) + "(sur)";
    while (curID != desID) {
//	    hr_log("\nlastId: %d\n", lastID);
        visited[curID] = 1;
        int nextID = selectNeighbour(curID, desID, pathStack.empty() ? -1 : pathStack.top(), visited);
        if (nextID == -1 && !pathStack.empty()) {      // 发生循环错误
            visited[curID] = -1;
//            hopNum--;
//            hr_log("[ <- %d ]\n", curID);
            curID = pathStack.top();
//            hr_log(" -> %d \n", curID);
            pathStack.pop();
            continue;
        }
        pathStack.push(curID);
//        hr_log("nextId: %d\n", nextID);
//        lastID = curID;
        hr_log(" -> %d", nextID);
        item.path += " -> ";
        item.path += to_string(nextID);
        if (hr_log_ON) {
            if (nextID < BASE * SCARE_FREE_NETWORK_SIZE) {
                printf("(sur)");
                item.path += "(sur)";
            } else if (nextID < BASE * (SCARE_FREE_NETWORK_SIZE + LOW_ORBIT_SIZE)) {
                printf("(low)");
                item.path += "(low)";
            } else if (nextID < BASE * (SCARE_FREE_NETWORK_SIZE + LOW_ORBIT_SIZE + MIDDLE_ORBIT_SIZE)) {
                printf("(mid)");
                item.path += "(mid)";
            } else {
                printf("(hig)");
                item.path += "(hig)";
            }
        }

        if (nextID == -1) {
            hr_log("\n[FALSE]: loop occured (%d, %d)\n\n", visited[nextID], nextID);
            item.result = "[FALSE]: loop occured";
            if (pHelper != nullptr) {
                pHelper->addItem(item);
            }
//            delete[] visited;
            return false;
        }
        totalDis += nodeList[curID]->GeographicDistanceTo(*nodeList[nextID]);
        hopNum++;
        curID = nextID;
    }
    item.result = "success";
    item.totalDistance = totalDis;
    item.hopNum = hopNum;
    if (pHelper != nullptr) {
        pHelper->addItem(item);
    }
//    int node_0 = 0;
//    int node_1 = 0;
//    int node_minus_1 = 0;
//    cout << endl;
//    for (auto item : visited) {
//        switch (item.second) {
//            case 0:
//                node_0++;
//                break;
//            case 1:
//                node_1++;
//                cout << "[ " << item.first << " ]";
//                printNeighbour(item.first);
//                break;
//            case -1:
//                node_minus_1++;
//                break;
//        }
//    }
//    printNeighbour(156);
//    printNeighbour(1633);
//
//    cout << endl;
//    hr_log("\n(0 -> %d, 1 -> %d, -1 -> %d)", node_0, node_1, node_minus_1);
    hr_log("\n[SUCCESS]: total distance: %.3f, hop num: %d\n\n", totalDis, hopNum);
//    delete[] visited;
    return true;
}

#define univeralDistribution(lower, upper) ((double)rand() / (double)RAND_MAX) * (upper - lower) + lower

//两个随机地面节点间的转发测试，两点间需要可达
bool RandomGraph::randomRoutingTest(LibXLHelper *pHelper) const {
    //DFS，验证des是否可达
    auto **searchStack = new ChainListNode *[SCARE_FREE_NETWORK_SIZE * BASE];
    bool visited[BASE * (SCARE_FREE_NETWORK_SIZE + LOW_ORBIT_SIZE + MIDDLE_ORBIT_SIZE + HIGH_ORBIT_SIZE)];
    bool accessible = false;
    int src = 0, des = 0;
    while (!accessible) {
        src = univeralDistribution(0, BASE * SCARE_FREE_NETWORK_SIZE);
        do {
            des = univeralDistribution(0, BASE * SCARE_FREE_NETWORK_SIZE);
        } while (des == src);
        for (int i = 0; i < nodeNum; i++)
            visited[i] = false;
        visited[src] = true;
        int Depth = 0;
        searchStack[Depth] = adjacencyTable[src];
        while (true) {
            if (searchStack[Depth] == NULL) {
                if (Depth == 0)
                    break;
                else {
                    Depth--;
                    searchStack[Depth] = searchStack[Depth]->next;
                }
            } else if (searchStack[Depth]->neighbourID == des) {
                accessible = true;
                printf("!\n");
                break;
            } else if (visited[searchStack[Depth]->neighbourID]) {
                searchStack[Depth] = searchStack[Depth]->next;
            } else {
                searchStack[Depth + 1] = adjacencyTable[searchStack[Depth]->neighbourID];
                visited[searchStack[Depth]->neighbourID] = true;
                Depth++;
            }
        }
    }
    return routingTest(src, des, pHelper);
}

void RandomGraph::printNeighbour(int id) const {
    auto *head = adjacencyTable[id];
    cout << endl;
    while (head != nullptr) {
        cout << head->neighbourID << " - ";
        head = head->next;
    }
    cout << endl;
}
