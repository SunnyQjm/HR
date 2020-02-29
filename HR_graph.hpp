#ifndef HR_GRAPH
#define HR_GRAPH

#include <map>
#include "HR_node.hpp"
#include "LibXLHelper.h"

//随机网络拓扑
class RandomGraph {
	//图信息
	int nodeNum; 
	Node** nodeList;

	//邻接表
	struct ChainListNode {
		int neighbourID;
		ChainListNode* next;
		ChainListNode(int ID = -1, ChainListNode* n = NULL) :
			neighbourID(ID), next(n) {}
	};
	ChainListNode** adjacencyTable;

public:
	//显示图信息
	void print() const {
		for (int i = 0; i < nodeNum; i++) {
			hr_log("ID: %d, ", i);
			nodeList[i]->print();
			hr_log("\n");
		}
	}

	//两个随机地面节点间的转发测试
    bool randomRoutingTest(LibXLHelper *pHelper) const;
	//转发测试,输入参数为始终点的ID
    bool routingTest(int srcID, int desID, LibXLHelper *pHelper) const;

	RandomGraph();
	~RandomGraph();
private:
	//给定当前节点ID与终点ID,依据双曲距离计算下一跳的节点ID
    int selectNeighbour(int curID, int desID, int i, std::map<int, int> map) const;
};

#endif //!HR_GRAPH
