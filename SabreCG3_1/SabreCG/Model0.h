#ifndef MODEL_H
#define MODEL_H

#include "Util.h"
#include "Leg.h"
#include "Aircraft.h"
#include "Station.h"
#include "Lof.h"
#include "OperLeg.h"

//* 原程序Model类的简化版本

class Model {
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
	vector<Lof* > _finalLofList;			// final selected lofs
	vector<Leg* > _cancelLegList;			// cancelled flights/maint

	static int _count;						// column generation的迭代次数

	vector<Leg *> _legList;                 // flight + maint
	vector<Aircraft *> _aircraftList;		// aircraft
	vector<Station *> _stationList;			// airport

	//* vector<Lof *> InitColumns
	vector<Lof* > _initColumns;				//* 初始column, 由findInitColumns成员函数初始

	vector<Leg *> _topOrderList;			//* leg topological order

	Leg _dummySource;						//* dummy source node connecting starting nodes // 使得findNewOneColumn的debug比较方便

public:
	//* Model(vector<Station *>, vector<Aircraft *>, vector<Leg *> , vector<Lof *>);
	Model(vector<Station *> stationList, vector<Aircraft *> aircraftList, vector<Leg *> legList, vector<Leg *> topOrderList);
	vector<Lof *> findNewColumns();		
	//*void findNewColumns();		// 先简单一点，只是显示，不保存

	Lof* findNewOneColumn(Aircraft* aircraft);
	void edgeProcessFltFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessFltMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessMaintFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessMaintMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);

	void edgeProcessFlt(Leg* nextLeg, Aircraft* aircraft);	// 起始flt设定
	void edgeProcessMaint(Leg* nextLeg, Aircraft* aircraft);	// 起始maint设定

	time_t computeFlightDelay (Leg* thisLeg, Leg* nextLeg); // 当nextLeg是flight时，计算delay, 考虑airport closure
	time_t delayByAirportClose (Leg* nextLeg, time_t delay); // 给定flight, 以及设定要加上的sequential delay，计算airport closure造成的delay

	vector<Lof* > solveColGen();
	vector<Lof* > findInitColumns();
	void populateByColumn(vector<Lof* > _initColumns);
	void solve();
	void addColumns(vector<Lof* > _betterColumns);	//*
	vector<Lof* > solveIP();
};

#endif