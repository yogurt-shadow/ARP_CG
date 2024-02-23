// Model for Multi-label DP 20170108
#ifndef MODEL_H
#define MODEL_H

#include "Util.h"
#include "Leg.h"
#include "Aircraft.h"
#include "Station.h"
#include "Lof.h"
#include "OperLeg.h"

#include <stdio.h>
#include <process.h>
#include <windows.h>


class Model
{
private:
	
	IloEnv _env;
	IloModel _model;
	//* IloModel _modelIP;
	IloCplex _solver;
	IloObjective _obj;

	IloNum _tolerance;

	// Var.
	IloNumVarArray _lofVar;
	IloNumVarArray _legVar;

	// Cons.
	IloRangeArray _coverRng;
	IloRangeArray _selectRng;				//* select one Lof for one aircraft

	// soln
	//vector<Lof* > _finalLofList;			// final selected lofs
	//vector<Leg* > _cancelLegList;			// cancelled flights/maint

	//static int _count;						// column generation的迭代次数
	int _count;

	static vector<Leg *> _legList;                 // flight + maint
	static vector<Aircraft *> _aircraftList;		// aircraft
	vector<Station *> _stationList;			// airport

	vector<Lof* > _initColumns;				//* 初始column, 由findInitColumns成员函数初始
	//static vector<Lof*> _betterColumns;
	static vector<Lof*> _betterColumns[THREADSIZE];

	static vector<Leg *> _topOrderList;			//* leg topological order

	double _lpObjVal;						//* 保存最终的LP目标函数值
	double _ipObjVal;						//* 保存最终的IP目标函数值
	int _lofSize;							//* 保存最终的lofSize

	int _lpTime;							//* accumulated time on solving the master problem
	int _spTime;							//* accumulated time on solving the subproblem
	int _ipTime;							//* time on solving the last IP

	//Leg _dummySource;						//* dummy source node connecting starting nodes // 使得findNewOneColumn的debug比较方便
public:
	Model(vector<Station *> stationList, vector<Aircraft *> aircraftList, vector<Leg *> legList, vector<Leg *> topOrderList);
	~Model();
	void findNewColumns();

	//Lof* findNewOneColumn(Aircraft* aircraft);
	static void findNewColumns(Aircraft* aircraft,int threadIndex);
	static unsigned int __stdcall findNewColumnsParallel(void * theNr);

	static void edgeProcessFltFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft,int threadIndex);
	static void edgeProcessFltFlt(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft, int threadIndex);

	static void edgeProcessFltMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft, int threadIndex);
	static void edgeProcessFltMaint(SubNode* thisLeg, Leg* nextLeg, Aircraft* aircraft, int threadIndex);

	static void edgeProcessMaintFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft, int threadIndex);
	static void edgeProcessMaintFlt(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft, int threadIndex);

	static void edgeProcessMaintMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft, int threadIndex);
	static void edgeProcessMaintMaint(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft, int threadIndex);

	static void edgeProcessFlt(Leg* nextLeg, Aircraft* aircraft, int threadIndex);
	static void edgeProcessMaint(Leg* nextLeg, Aircraft* aircraft, int threadIndex);

	static time_t computeFlightDelay (SubNode* subNode, Leg* nextLeg);
	static time_t delayByAirportClose (Leg* nextLeg, time_t delay);

	void clearBetterColumnsAll();
	bool hasBetterColumn();

	vector<Lof* > solveColGen();
	vector<Lof* > findInitColumns();
	Lof* findInitOneColumn(Aircraft* aircraft);
	void populateByColumn(vector<Lof* > _initColumns);
	void solve();
	void addColumns(vector<Lof* > _betterColumns);	//*
	vector<Lof* > solveIP();

	double getLpObjValue(){return _lpObjVal;}
	double getIpObjValue(){return _ipObjVal;}
	int getCount(){return _count;}
	int getLofSize(){return _lofSize;}
	int getLpTime(){return _lpTime;}
	int getSpTime(){return _spTime;}
	int getIpTime(){return _ipTime;}
};

#endif