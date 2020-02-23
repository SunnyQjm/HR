#ifndef HR_CONFIG
#define HR_CONFIG

#include <cstdio>
#include <cmath>

#define PI 3.1415926
#define EARTH_RADIUS 6371 //地球半径,单位千米

#define BIG_ENOUGH 0xFFFFFFFF //足够大的正数,用于代替无穷大

//图生成器参数
#define SCARE_FREE_NETWORK_AVERAGE_DEGREE 4.92 //无标度网络的平均度数
#define BASE 1
//图中各种类节点的数目= BASE x SIZE
#define SCARE_FREE_NETWORK_SIZE 2000 //bh无标度网络（地、空 节点）,目前比例是13:5的时候成功率才高一些
#define LOW_ORBIT_SIZE 72 //bh低轨
#define MIDDLE_ORBIT_SIZE 18 //bh中轨
#define HIGH_ORBIT_SIZE 3 //bh高轨
//各种类卫星的轨道半径
#define LOW_ORBIT_RADIUS 7746
#define MIDDLE_ORBIT_RADIUS 22371
#define HIGH_ORBIT_RADIUS 42157
//各种类卫星对应的双曲半径           //bh调参重点对象-能否路由成功的关键点就在于有没有合理的双曲半径设置方法
#define LOW_ORBIT_HYPERRADIUS 5.7
#define MIDDLE_ORBIT_HYPERRADIUS 5.3
#define HIGH_ORBIT_HYPERRADIUS 4.1

//双曲坐标映射用参数
//球极射影时使用的球半径,应当与宇宙距离同一个数量级,单位千米
#define RADIUS_FOR_STEREGRAPHIC_PROJECTION 40000 
//节点分级,用于指定双曲坐标,将节点按通信半径从高至低排列,比例介于RANKm_PROPORTION与RANK(m+1)_PROPORTION间的节点划归RANKm
#define RANK1_PROPORTION 0.0625
#define RANK2_PROPORTION 0.125
#define RANK3_PROPORTION 0.25
#define RANK4_PROPORTION 0.5
//各RANK对应的双曲半径
#define EPSILON 30
#define RANK1_HYPERRADIUS log(EPSILON + RANK1_PROPORTION * SCARE_FREE_NETWORK_SIZE * BASE)
#define RANK2_HYPERRADIUS log(EPSILON + RANK2_PROPORTION * SCARE_FREE_NETWORK_SIZE * BASE)
#define RANK3_HYPERRADIUS log(EPSILON + RANK3_PROPORTION * SCARE_FREE_NETWORK_SIZE * BASE)
#define RANK4_HYPERRADIUS log(EPSILON + RANK4_PROPORTION * SCARE_FREE_NETWORK_SIZE * BASE)
#define RANK5_HYPERRADIUS log(EPSILON + SCARE_FREE_NETWORK_SIZE * BASE)



#define hr_debug_ON false
#define hr_log_ON true



#if hr_log_ON
#define hr_log(M, ...) printf(M, ##__VA_ARGS__)
#else
#define hr_log(M, ...)
#endif //!hr_log_ON

#if hr_debug_ON
#define hr_debug(M, ...) printf(M, ##__VA_ARGS__)
#else
#define hr_debug(M, ...)

#endif //!hr_log_ON
#endif //!HR_CONFIG
