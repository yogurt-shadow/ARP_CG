#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include"Util.h"
#include"Station.h"
#include"Leg.h"
//* class Lof;
//* class AircraftNode;

class Aircraft {
private:
	string _tail;                           // Tail Number
	int _id;                                // Number of aircraft

	static int _count;

	time_t _startT;                        // Start Available Time
	time_t _endT;                          // End Available Time

	Station * _depStation;                 // Start Available Station
	Station * _arrStation;                 // End Available Station

	double _dual;							//* 对应master problem每架飞机只选一条Lof的constraint

	vector<Leg *> _planLegList;            // flights assigned to aircraft by schedule
	/*
	vector<Leg *> _realLegList;            // flights assigned to aircraft by model solution
	vector<Leg *> _maintList;              // maintenanceList the aircraft should carry

	vector<Lof *> _lofList;                // candidate lofs that can be assigned to aircraft
	vector<Lof *> _addLofList;             ///????????????????????????????????

	vector<AircraftNode *> _nodeList;      /// 每一架飞机自带_nodeList 记录在各个机场的时间点

	bool _isMaint;
	*/
public:
	Aircraft(string name, time_t startT, time_t endT, Station * depS, Station * arrS);

	void pushPlanLeg(Leg * leg) {_planLegList.push_back(leg);}

	string getTail() {return _tail;}

	//* void pushMaint(Leg * maint) {_maintList.push_back(maint);}

	void print();

	//* int getAvaibleDate() {return (_startT-DATECUTTIME)/DATEDURATION;}

	Station * getDepStation() {return _depStation;}
	Station * getArrStation() {return _arrStation;}

	time_t getStartTime() {return _startT;}
	time_t getEndTime() {return _endT;}

	int getId() {return _id;}

	void setDual(double dual) {_dual = dual;}					//*
	double getDual() {return _dual;}							//*

	vector<Leg* > getPlanLegList() {return _planLegList;}		//*
	void sortScheLegByDepTime();                              //* sort the scheduled flights by their arrival time
	bool isPlanLegFeasible();									//* check whether planned leg list feasible

	/*
	void pushLof(Lof * lof) {_lofList.push_back(lof);}

	Lof * getFirstDepLof() {return _lofList.front();}         // the earlist candidate lof flew by aircraft
	Leg * getLastScheLeg() {return _planLegList.back();}      // the last planned flight of the aircraft

	vector<Lof *> getLofList() {return _lofList;}

	void sortScheLegByArrTime();                              // sort the scheduled flights by their arrival time
	void sortLofByDepTime();                                  // sort all the candidate lofs by their departure time

	vector<AircraftNode *> getNodeList(){return _nodeList;}   ///????????????????????????????????

	static bool compareTime(Aircraft * a,Aircraft * b);       ///????????????????????????????????

	void initNodes();                /// 根据aircraft的lof初始化time node (新建time node，和schedule里建的没有关系)
	void sortNodes();                ///????????????????????????????????

	void delayLofsCrossDay();        ///????????????????????????????????

	void addNodes();                 ///????????????????????????????????
	*/
};

#endif