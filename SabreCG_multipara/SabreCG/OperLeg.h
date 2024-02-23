#ifndef OPER_LEG_H
#define OPER_LEG_H

#include "util.h"
#include "Leg.h"
#include "Aircraft.h"

class OperLeg
{
private:
	Leg * _leg;						// original leg
	Aircraft * _operAircraft;		// operational aircraft
	time_t _depTime;				// OPERATIONAL departure time
	time_t _arrTime;                // OPERATIONAL arrival time

public:

	OperLeg(Leg * leg);
	OperLeg(Leg * leg, Aircraft * aircraft);	//* 用在subproblem生成LoF时，直接指定aircraft
	~OperLeg(){}

	//* double getCost();

	void print();

	Leg * getLeg() {return _leg;}

	//* OperLeg * clone();

	time_t getOpDepTime();
	time_t getOpArrTime() {return _arrTime;}
	
	time_t getPrintDepTime() {return _depTime;}
	time_t getPrintArrTime() {return _arrTime;}

	void setOpDepTime(time_t t) {_depTime = t;}
	void setOpArrTime(time_t t) {_arrTime = t;}

	time_t getScheDepTime() {return _leg->getDepTime();}
	time_t getScheArrTime() {return _leg->getArrTime();}

	//* void delayTo(time_t time);  

	void setOpAircraft(Aircraft * aircraft) {_operAircraft = aircraft;}

	//* time_t delayByAirportClose();

	Aircraft * getOperAircraft() {return _operAircraft;}
	Aircraft * getScheAircraft() {return _leg->getAircraft();}
};

#endif