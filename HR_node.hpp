#ifndef HR_NODE
#define HR_NODE

#include "HR_coordinate.hpp"

//网络节点
class Node {
public:
    //类别:地,天,空
    enum Type {
        surficial, aerial, cosmic
    };
private:
    //节点固有特征
    Type type;
    double range; //通信最远距离,通信半径

    //坐标
    SphericalCoordinate3D position; //三位球坐标,在宇宙中的位置
    SphericalCoordinate4D hyperPosition; //四维球坐标,双曲坐标
public:
    //检查是否可以与另一节点通信
    bool checkIfConnectTo(const Node &) const;

    //计算与另一节点的双曲距离
    double HyperbolicDistanceTo(const Node &a) const {
        return hyperPosition.HyperbolicDistanceTo(a.hyperPosition);
    }

    //计算与另一节点的几何距离
    double GeographicDistanceTo(const Node &a) const {
        return position.EuildeanDistanceTo(a.position);
    }

    void print() const {
        hr_log("type:");
        switch (type) {
            case Node::surficial:
                hr_log("surficial,");
                break;
            case Node::aerial:
                hr_log("aerial,");
                break;
            case Node::cosmic:
                hr_log("cosmic,");
                break;
        }
        hr_log(" range: %.2f, position:", range);
        position.print();
    }

    friend class NodeGenerator;
};

//随机点生成器
class NodeGenerator {
public:
    //生成函数
    static Node &randomSurficialNode();

    static Node &randomLowOrbitNode();

    static Node &randomMiddleOrbitNode();

    static Node &randomHighOrbitNode();

private:
    NodeGenerator();

    //以下为生成过程中中间变量
    static const double NodeDensity; //节点密度, 地球表面每平方公里拥有的节点数目
    static const double Gamma; //节点度数所服从幂率分布中的参数
    //buffer,记录各rank对应的通信半径阈值
    static double rankThreshold[4];
    //各Rank对应的双曲半径
    static double rankHyperRadius[5];

    //随机生成通信半径,需使得节点度数满足幂率分布
    static double generateRange();

    //前xxx比例对应的通信半径,参数范围[0, 1]
    static double rangeToCumulative(double);
};

#endif //!HR_NODE)