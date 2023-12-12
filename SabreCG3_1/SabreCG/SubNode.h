#ifndef SUBNODE_H
#define SUBNODE_H

#include"Util.h"
//#include "Leg.h"
class Leg;

class SubNode
{
private:
	Leg* _leg;						// save the leg that the subnode belongs to
	SubNode* _parentSubNode;		// the previous subnode
	double _subNodeCost;
	time_t _delay;						// ��¼��DP�е�����delay

	//time_t _operDepTime;				//* ��¼��DP�е������flight departure time
	//time_t _operArrTime;				//* ��¼��DP�е������flight arrival time

public:
	SubNode(Leg* leg, SubNode* parentSubNode, double subNodeCost, time_t delay);
	~SubNode(){}

	int getLegId();
	int getParentLegId();
	Leg* getLeg() {return _leg;}
	void setLeg(Leg* leg) {_leg = leg;}

	SubNode* getParentSubNode() {return _parentSubNode;}
	void setParentSubNode(SubNode* parentSubNode) {_parentSubNode = parentSubNode;}

	double getSubNodeCost() {return _subNodeCost;}
	void setSubNodeCost(double subNodeCost) {_subNodeCost = subNodeCost;}

	time_t getDelay() {return _delay;}
	void setDelay(time_t delay) {_delay = delay;}

	void print();

	time_t getOperDepTime();		// ����hosting leg��Operational Departure Time
	time_t getOperArrTime();		// ����hosting leg��Operational Arrival Time

	static bool lessSubNodePointer(SubNode* p1, SubNode* p2);

	static bool cmpByCost(SubNode* a, SubNode* b);
};

#endif