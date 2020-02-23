#include <cstdlib>
#include "HR_node.hpp"

bool Node::checkIfConnectTo(const Node& a) const {
	double distance = position.EuildeanDistanceTo(a.position);
	if (distance < range && distance < a.range) {
		//若是大气层内的两个节点进行通信,由于电磁层反射,暂不考虑地球曲率带来的影响
		if (type != cosmic && a.type != cosmic)
			return true;

		//若涉及空间节点,则需要检查两个节点的连线是否被地球遮挡

		/*
		  由于两节点的球心距均大于地球半径,两个节点的连线被地球遮挡当且仅当:
		  1) 地心到两节点连线的垂线长度小于地球半径
		  2) 地心到两节点连线的垂点在两节点之间

		  反之,若两节点间的线段不经过垂点,则代表未被地球遮挡
		  记两节点的笛卡尔坐标分别为(x1, y1, z1)与(x2, y2, z2), 则过原点的两点连线的切平面为 (x1 - x2)x + (y1 - y2)y + (z1 - z2)z = 0, 垂点即在此平面上
		  若两节点在平面的同一侧,则代表不经过垂点
		  即 [(x1 - x2)x1 + (y1 - y2)y1 + (z1 - z2)z1] * [(x1 - x2)x2 + (y1 - y2)y2 + (z1 - z2)z2] > 0
		*/

		double x1 = position.radius * cos(position.theta);
		double y1 = position.radius * sin(position.theta) * cos(position.phi);
		double z1 = position.radius * sin(position.theta) * sin(position.phi);
		double x2 = a.position.radius * cos(a.position.theta);
		double y2 = a.position.radius * sin(a.position.theta) * cos(a.position.phi);
		double z2 = a.position.radius * sin(a.position.theta) * sin(a.position.phi);

		if (((x1 - x2) * x1 + (y1 - y2) * y1 + (z1 - z2) * z1) * ((x1 - x2) * x2 + (y1 - y2) * y2 + (z1 - z2) * z2) > 0)
			return true;

		//若两节点在垂平面的两侧，则检查两点连线距球心的最短距离是否小于地球半径，若是，则连线会被地球遮挡
		double dx = x2 - x1;
		double dy = y2 - y1;
		double dz = z2 - z1;
		double lambda = -(x1 * dx + y1 * dy + z1 * dz) / (dx * dx + dy * dy + dz * dz);
		double x_star = x1 + lambda * dx;
		double y_star = y1 + lambda * dy;
		double z_star = z1 + lambda * dz;
		if (sqrt(x_star * x_star + y_star * y_star + z_star * z_star) > EARTH_RADIUS)
			return true;
	}
	return  false;
}

#define univeralDistribution(lower, upper) ((double)rand() / (double)RAND_MAX) * (upper - lower) + lower

double NodeGenerator::rangeToCumulative(double c) {
	//通信范围在地球表面上的圆面积
	double area = (pow(c, Gamma / (1 - Gamma)) + Gamma - 1) / (Gamma * NodeDensity);
	return sqrt(area / PI);
}

double NodeGenerator::generateRange() {
	return rangeToCumulative(univeralDistribution(0, 1));
}

Node& NodeGenerator::randomSurficialNode() {
	//随机生成地表节点
	Node* n = new Node;

	//球坐标上,两个角分量需满足均匀分布
	double theta = univeralDistribution(0, PI);
	double phi = univeralDistribution(0, 2 * PI);

	//通信范围应当满足网络的无标度性:
	double range = generateRange();
	//由于为地面节点,球心距为地球半径
	double radius = EARTH_RADIUS;
	//球极射影获得双曲坐标中的第三个角分量
	double psi = 2 * atan(radius / RADIUS_FOR_STEREGRAPHIC_PROJECTION);
	//获得双曲半径
	int i = 0;
	while (i < 4 && range < rangeToCumulative(rankThreshold[i])) {
		i++; 
	}
	double hyperRadius = rankHyperRadius[i];
	n->type = Node::surficial;
	n->position = SphericalCoordinate3D(radius, theta, phi);
	n->hyperPosition = SphericalCoordinate4D(hyperRadius, theta, phi, psi);
	n->range = range;
	return *n;
}

Node& NodeGenerator::randomLowOrbitNode() {
	Node* n = new Node;

	double theta = univeralDistribution(0, PI);
	double phi = univeralDistribution(0, 2 * PI);

	//通信范围应当满足网络的无标度性:
	double range = BIG_ENOUGH;
	//由于为地面节点,球心距为地球半径
	double radius = LOW_ORBIT_RADIUS;
	//球极射影获得双曲坐标中的第三个角分量
	double psi = 2 * atan(radius / RADIUS_FOR_STEREGRAPHIC_PROJECTION);
	//获得双曲半径
	double hyperRadius = LOW_ORBIT_HYPERRADIUS;

	n->type = Node::cosmic;
	n->position = SphericalCoordinate3D(radius, theta, phi);
	n->hyperPosition = SphericalCoordinate4D(hyperRadius, theta, phi, psi);
	n->range = range;
	return *n;
}

Node& NodeGenerator::randomMiddleOrbitNode() {
	Node* n = new Node;

	double theta = univeralDistribution(0, PI);
	double phi = univeralDistribution(0, 2 * PI);

	//通信范围应当满足网络的无标度性:
	double range = BIG_ENOUGH;
	//由于为地面节点,球心距为地球半径
	double radius = MIDDLE_ORBIT_RADIUS;
	//球极射影获得双曲坐标中的第三个角分量
	double psi = 2 * atan(radius / RADIUS_FOR_STEREGRAPHIC_PROJECTION);
	//获得双曲半径
	double hyperRadius = MIDDLE_ORBIT_HYPERRADIUS;

	n->type = Node::cosmic;
	n->position = SphericalCoordinate3D(radius, theta, phi);
	n->hyperPosition = SphericalCoordinate4D(hyperRadius, theta, phi, psi);
	n->range = range;
	return *n;
}

Node& NodeGenerator::randomHighOrbitNode() {
	Node* n = new Node;

	double theta = 0;
	double phi = univeralDistribution(0, 2 * PI);

	//通信范围应当满足网络的无标度性:
	double range = BIG_ENOUGH;
	//由于为地面节点,球心距为地球半径
	double radius = HIGH_ORBIT_RADIUS;
	//球极射影获得双曲坐标中的第三个角分量
	double psi = 2 * atan(radius / RADIUS_FOR_STEREGRAPHIC_PROJECTION);
	//获得双曲半径
	double hyperRadius = HIGH_ORBIT_HYPERRADIUS;

	n->type = Node::cosmic;
	n->position = SphericalCoordinate3D(radius, theta, phi);
	n->hyperPosition = SphericalCoordinate4D(hyperRadius, theta, phi, psi);
	n->range = range;
	return *n;
}

const double NodeGenerator::NodeDensity = SCARE_FREE_NETWORK_SIZE * BASE / (4 * PI * EARTH_RADIUS * EARTH_RADIUS);
const double NodeGenerator::Gamma = (2 * SCARE_FREE_NETWORK_AVERAGE_DEGREE - 1) / (SCARE_FREE_NETWORK_AVERAGE_DEGREE - 1);
double NodeGenerator::rankThreshold[4] = { RANK1_PROPORTION, RANK2_PROPORTION, RANK3_PROPORTION, RANK4_PROPORTION };
double NodeGenerator::rankHyperRadius[5] = { RANK1_HYPERRADIUS, RANK2_HYPERRADIUS, RANK3_HYPERRADIUS, RANK4_HYPERRADIUS, RANK5_HYPERRADIUS };


