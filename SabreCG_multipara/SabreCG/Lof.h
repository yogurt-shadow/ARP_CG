#ifndef _LOF_H
#define _LOF_H

#include "Leg.h"
#include "OperLeg.h"
#include "Aircraft.h"

class Lof
{
private:
	Aircraft * _aircraft;             // operational aircraft assignmed to lof
	vector<OperLeg *> _legList;       // flightList after delay and swap

	vector<OperLeg *> _maintList;     // maintList in the Lof

	double _cost;                     // total cost of the lof = delay + swap
	//* int _purity;

	//* AircraftNode * _depNode;          
	//* AircraftNode * _arrNode;

	double _reducedCost;              // _reducedCost of the column

	int _id;

	//* bool _isSelected;

	static int _count;

public:

	Lof();
	~Lof();

	void pushLeg(OperLeg * leg);
	void popLeg() {delete _legList.back(); _legList.pop_back();}             /// popLeg() ���_legList���OperLeg delete������ͬlof��Ķ�Ӧͬһ��leg��Operleg�ǻ��������
	int getSize() {return _legList.size();}

	void setLegList(vector<OperLeg *> legList) {_legList = legList;}
	vector<OperLeg *> getLegList() {return _legList;}

	void setAircraft(Aircraft * aircraft) {_aircraft = aircraft;}
	Aircraft* getAircraft(){return _aircraft;}


	//* void compCostWithoutPopulate();
	//* void conpCostWithPopulate();           /// ������compCostWithPopulate()
	//* void compPurity();
	void computeLofCost();					//*

	bool containMaint() {return !_maintList.empty();}

	void setCost(double cost) {_cost = cost;}
	double getCost() {return _cost;}

	/*
	void setPurity(int purity) {_purity = purity;}
	int getPurity(){return _purity;}
	*/

	//* Lof * clone();         /// ����Lof

	OperLeg * getLastLeg() {return _legList.back();} 

	void print();

	//* bool compatible(Aircraft * aicraft);

	/*
	bool rescheduleTime(time_t t);                         /// ????????????????????????????????????? ���Ǵ���sequential delay��?
	bool rescheduleTime();
	bool rescheduleTimeWithMaint();
	bool rescheduleTimeWithoutMaint();
	bool rescheduleTimeWithMaint(time_t prevEndTime);
	bool rescheduleTimeWithoutMaint(time_t prevEndTime);
	void rescheduleTimeWithoutMaintAirportClose();
	void rescheduleTimeWithMaintAirportClose();
	*/

	Station * getDepStation() { return _legList.front()->getLeg()->getDepStation();}
	Station * getArrStation() { return _legList.back()->getLeg()->getArrStation();}

	time_t getOperDepTime();
	time_t getOperArrTime();

	time_t getDepTime();

	/*
	void setDepNode(AircraftNode * node) {_depNode = node;}
	void setArrNode(AircraftNode * node) {_arrNode = node;}

	AircraftNode * getDepNode() { return _depNode;}
	AircraftNode * getArrNode() { return _arrNode;}
	*/

	static bool compareDepTime(Lof * a, Lof * b);
	static bool compareByAircraft(Lof* a, Lof* b);			//* ȷ��multi-threadÿ�μӽ�master problem��lof˳��һ��

	//* void computerReducedCost();
	void computeReducedCost();		//*
	double getReducedCost() {return _reducedCost;}

	void setId(int id){_id=id;}
	int getId() {return _id;}
	
	/*
	void setIsSelected(bool _bool){ _isSelected = _bool;}
	bool getIsSelected() {return _isSelected;}
	*/
};

#endif