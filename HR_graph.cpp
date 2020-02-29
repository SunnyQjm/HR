#include <cstdlib>
#include <iostream>
#include "HR_graph.hpp"
#include "LibXLHelper.h"
#include <map>
#include <stack>

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
    double edge_num = 0;
    //计算邻接表
    ChainListNode *n;
    for (int i = 0; i < nodeNum; i++)
        for (int j = i + 1; j < nodeNum; j++) {
            if (nodeList[i]->checkIfConnectTo(*nodeList[j])) {
                //i,j相连
                n = new ChainListNode(j, adjacencyTable[i]);
                adjacencyTable[i] = n;
                n = new ChainListNode(i, adjacencyTable[j]);
                adjacencyTable[j] = n;
                edge_num += 2;
            }
        }
    hr_log("average degree is %.2f\n", edge_num / nodeNum);
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
//	    cout << map[t->neighbourID] << " - ";
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
    for (int i = 0; i < nodeNum; i++)
        visited[i] = 0;
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
        if(nextID == -1 && !pathStack.empty()) {      // 发生循环错误
            visited[curID] = -1;
            curID = pathStack.top();
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
            if(pHelper != nullptr) {
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
    if(pHelper != nullptr) {
        pHelper->addItem(item);
    }
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