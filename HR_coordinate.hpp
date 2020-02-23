#ifndef HR_COORDINATE
#define HR_COORDINATE

#include "HR_config.hpp"

using namespace std;

//三维空间中的球坐标，在此例中，该坐标代表网络节点在宇宙空间中的位置
struct SphericalCoordinate3D {
private:
	double radius; //球坐标中半径分量,取值范围[0, ∞)
	double theta; //球坐标中的极角分量，取值范围[0, π)
	double phi; //球坐标中的方位角分量，取值范围[0, 2π)
public:	
	//与另一个点的球心角(的余弦值)
	double CosineInnerAngleTo(const SphericalCoordinate3D& a) const{
		return (cos(theta) * cos(a.theta) +
			sin(theta) * sin(a.theta) * cos(phi - a.phi));
	}

	//到另一个点的欧式距离(直线距离)
	double EuildeanDistanceTo(const SphericalCoordinate3D& a) const {
		return sqrt(radius * radius + a.radius * a.radius - 
			2 * radius * a.radius * CosineInnerAngleTo(a));
	}
	void print() const {
		hr_log("(%.0f,%.2f,%.2f)", radius, theta, phi);
	}
	SphericalCoordinate3D(double r = EARTH_RADIUS, double th = 0, double ph = 0)
		:radius(r), theta(th), phi(ph) {}
	friend class Node;
};

//四维空间中的球坐标，在此例中，该坐标代表网络节点在四维庞加莱超球中的位置
//其中的三个角坐标分量来自于宇宙空间中的位置，径坐标分量来自于该节点在网络中的中心化程度
struct SphericalCoordinate4D {
private:
	double theta;
	double phi; //theta和phi直接继承自三维坐标的同名分量
	double psi; //此分量来自于三位坐标中的radius，通过球极投影完成运算,取值范围[0, π)
	double hyperRadius; //半径分量，在网络中处于越中心位置的节点，半径越小，取值范围[0, ∞)
public:
	//到另一个点的双曲距离
	double HyperbolicDistanceTo(const SphericalCoordinate4D& b) const {
		double CosineInnerAngle = cos(theta) * cos(b.theta) + sin(theta) * sin(b.theta) * 
			(cos(phi) * cos(b.phi) + sin(phi) * sin(b.phi) * cos (psi - b.psi));
		if (CosineInnerAngle > 0.9999999)
			return 0;
		return acosh(cosh(hyperRadius) * cosh(b.hyperRadius) - 
			sinh(hyperRadius) * sinh(b.hyperRadius) * CosineInnerAngle);
	}
	void print() const {
		hr_log("(%.0f,%.2f,%.2f,%.2f)", hyperRadius, theta, phi, psi);
	}
	SphericalCoordinate4D(double r = RANK5_HYPERRADIUS, double th = 0, double ph = 0, double ps = 0)
		:hyperRadius(r), theta(th), phi(ph), psi(ps) {}
	friend class Node;
};
#endif //!HR_COORDINATE
