#ifndef LEG_H
#define LEG_H

#include"Util.h"
#include"Station.h"
#include"Aircraft.h"
#include"SubNode.h"

/*ԭʼ������Ϣ*/
/*leg����flight��maitenance*/
class Leg {
private:

	Station * _depStation;
	Station * _arrStation;

	Aircraft * _aircraft;

	time_t _depTime;
	time_t _arrTime;

	bool _isVisited;
	bool _isAssigned;

	string _flightNum;
	int _id;
	static int _count;

	bool _isMaint;

	vector<Leg *> _prevLegList;			// neighbor list of previous legs
	vector<Leg *> _nextLegList;			// neighbor list of next legs

	double _dual;						// dual variable from master problem

	vector<SubNode* > _subNodeList[THREADSIZE];		// ����multi label

	//Leg * _topOrderPrev;				//* previous leg in topological order
	//Leg * _topOrderNext;				//* next leg in topological order

	//time_t _operDepTime;				//* multi-lable����leg�м�¼operDepTime //* ��¼��DP�е������flight departure time
	//time_t _operArrTime;				//* multi lable����leg�м�¼operArrTime //* ��¼��DP�е������flight arrival time

	//Leg * _parent;						//* save preceding node of in dynamic programming of shortest path
	//time_t _delay;						//* the operational delay of this flight //* ��_operDepTime, _operArrTime���
	//double _nodeCost;					//* node cost of DP
	//time_t _accDelay;					//* accumulated delay in DP
	//double _accDual;					//* accumulated dual in DP
	//* double _lp

public:
	Leg(string flightNum, Station * depStation, Station * arrStation, time_t depTime, time_t arrTime, Aircraft * aircraft);
	Leg(string flightNum);				//* Ϊdummy node����дconstructor, ���ı�leg�ļ���

	void print();

	void setId(int id){_id = id;}
	int getId() {return _id;}

	string getFlightNum() {return _flightNum;}

	Station * getArrStation() {return _arrStation;}
	Station * getDepStation() {return _depStation;}

	time_t getArrTime() {return _arrTime;}
	time_t getDepTime() {return _depTime;}

	Aircraft * getAircraft() {return _aircraft;}

	void setPrevLegList(vector<Leg *> legList) {_prevLegList = legList;}
	void setNextLegList(vector<Leg *> legList) {_nextLegList = legList;}

	vector<Leg *> getPrevLegList() {return _prevLegList;}
	vector<Leg *> getNextLegList() {return _nextLegList;}

	void pushNextLeg(Leg * leg) {_nextLegList.push_back(leg);}
	void pushPrevLeg(Leg * leg) {_prevLegList.push_back(leg);}

	bool isMaint() {return _isMaint;}
	void setIsMaint(bool _b) {_isMaint = _b;}

	bool isVisited() {return _isVisited;}				//*
	void setIsVisited(bool _b) {_isVisited = _b;}			//*

	//* int getDate();

	void setDual(double dual) {_dual = dual;}
	double getDual() {return _dual;}

	//* static bool compareArrTime(Leg * a, Leg * b);
	static bool compareDepTime(Leg * a, Leg * b);	//*
	
	void setAssigned(bool _b){_isAssigned = _b;}
	bool getAssigned() {return _isAssigned;}
	

	//void setTopOrderPrev(Leg * topOrderPrev){ _topOrderPrev = topOrderPrev;}	//*
	//Leg * getTopOrderPrev() {return _topOrderPrev;}								//*

	//void setTopOrderNext(Leg * topOrderNext){ _topOrderNext = topOrderNext;}	//*
	//Leg * getTopOrderNext() {return _topOrderNext;}								//*

	//void setNodeCost(double cost){ _nodeCost = cost;}		//*
	//double getNodeCost(){return _nodeCost;}					//*

	//void setOperDepTime(time_t time){ _operDepTime = time;}		//*
	//time_t getOperDepTime(){ return _operDepTime;}				//*

	//void setOperArrTime(time_t time){ _operArrTime = time;}		//*
	//time_t getOperArrTime(){ return _operArrTime;}				//*

	//void setParent(Leg* leg){ _parent = leg;}		//*
	//Leg* getParent(){return _parent;}					//*

	//bool resetLeg(); //* ����Leg��_nodeCost, _operDepTime, _operArrTime, _parent, _accDual, _accDelay
	bool resetLeg(int threadIndex); //* ɾ��_subNodeList������ָ��ָ���subNode, ��ɾ��_subNodeList���е�ָ��

	vector<SubNode* > getSubNodeList(int threadIndex) {return _subNodeList[threadIndex];}

	void pushSubNode(SubNode* subNode, int threadIndex) {_subNodeList[threadIndex].push_back(subNode);}
	void popSubNode(int threadIndex) {delete _subNodeList[threadIndex].back(); _subNodeList[threadIndex].pop_back();}
	bool insertSubNode(SubNode* subNode, int threadIndex);	//* ��subNode��subNodeList�е�subNodeһһ�Ƚ��Ƿ�dominate������subNodeList

	//void setAccDual(double accDual){_accDual = accDual;}			//*
	//double getAccDual() {return _accDual;}							//*

	//void setAccDelay(time_t accDelay){_accDelay = accDelay;}		//*
	//time_t getAccDelay() {return _accDelay;}						//*

	//void setDelay(time_t delay){_delay = delay;}					//*
	//time_t getDelay() {return _delay;}								//*
};

#endif