#ifndef STATION_H
#define STATION_H

#include "Util.h"

class Leg;
//* class AircraftNode;
class Aircraft;

class Station {
private:
	string _name;                                  // station name
	int _id;                                       // number of station //* Station的顺序编号

	static int _count;

	vector<Aircraft *> _depAircraft;               // aircrafts depart from the station ///以机场为起始机场的aircraft
	vector<Leg *> _depLegList;                     // flights depart from the station
	vector<Leg *> _arrLegList;                     // flights arrive at the station

	vector<Leg *> _maintList;                      // maintenance carried at the station
	//* vector<AircraftNode *> _nodeList;

	
	//* int _terminateAircraftCount;                   // number of aircraft terminate at the station by schedule

	vector<pair<time_t, time_t>> _closeTimeList;   // close time of the airport if there are airport closures

	//* pair<double, double> _dual;

	//* double _arrLegPro;                   ///???????????????????????????????? /// arrving leg proportion?          
	//* double _depLegPro;                   ///???????????????????????????????? /// departure leg proportion? 

	size_t _legNum;                      ///???????????????????????????????? /// 问题的航班总数 leg number
	//* time_t _StartTime;                   ///????????????????????????????????
	
public:
	Station(string _name);

	string getName() {return _name;}

	void pushDepLeg(Leg * leg) {_depLegList.push_back(leg);}
	void pushArrLeg(Leg * leg) {_arrLegList.push_back(leg);}

	
	void pushDepAircraft(Aircraft * _Aircraft){_depAircraft.push_back(_Aircraft);}
	/*
	vector<Aircraft *> getDepAircraft(){return _depAircraft;}
	void sortAircraftByDepTime();
	*/
	int getDepAircraftNumber(){return _depAircraft.size();}

	vector<Leg *> getDepLegList() {return _depLegList;}
	vector<Leg *> getArrLegList() {return _arrLegList;}

	void pushMaint(Leg * maint) {_maintList.push_back(maint);}
	vector<Leg *> getMaintList() {return _maintList;}

	
	void pushCloseTime(pair<time_t, time_t> closeTime) {_closeTimeList.push_back(closeTime);}// set the closure time of the station
	vector<pair<time_t, time_t> > getCloseTimeList() {return _closeTimeList;}

	void print();
	
	/*
	void pushNode(AircraftNode * node) {_nodeList.push_back(node);}

	void sortNode();

	void printNodes();

	void increaseTerminalCount() {_terminateAircraftCount++;}

	int getTerminalCount() {return _terminateAircraftCount;}

	void setTerminalCount(int count) {_terminateAircraftCount = count;}
	*/

	int getId() {return _id;}

	//* void setDual(pair<double, double> dual) {_dual = dual;}   ///为什么dual是pair<double, double>?
	//* pair<double, double> getDual() {return _dual;}

	void setLegNum(size_t _size){ _legNum = _size;}   ///????????????????????????????????
	/*
	void setLegPro();                                 ///????????????????????????????????
	
	double getArrLegPro(){return _arrLegPro;}  ///????????????????????????????????
	double getDepLegPro(){return _depLegPro;}  ///????????????????????????????????

	int getDate();
	*/
};

#endif