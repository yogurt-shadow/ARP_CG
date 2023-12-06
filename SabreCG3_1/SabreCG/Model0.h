#ifndef MODEL_H
#define MODEL_H

#include "Util.h"
#include "Leg.h"
#include "Aircraft.h"
#include "Station.h"
#include "Lof.h"
#include "OperLeg.h"

//* ԭ����Model��ļ򻯰汾

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

	static int _count;						// column generation�ĵ�������

	vector<Leg *> _legList;                 // flight + maint
	vector<Aircraft *> _aircraftList;		// aircraft
	vector<Station *> _stationList;			// airport

	//* vector<Lof *> InitColumns
	vector<Lof* > _initColumns;				//* ��ʼcolumn, ��findInitColumns��Ա������ʼ

	vector<Leg *> _topOrderList;			//* leg topological order

	Leg _dummySource;						//* dummy source node connecting starting nodes // ʹ��findNewOneColumn��debug�ȽϷ���

public:
	//* Model(vector<Station *>, vector<Aircraft *>, vector<Leg *> , vector<Lof *>);
	Model(vector<Station *> stationList, vector<Aircraft *> aircraftList, vector<Leg *> legList, vector<Leg *> topOrderList);
	vector<Lof *> findNewColumns();		
	//*void findNewColumns();		// �ȼ�һ�㣬ֻ����ʾ��������

	Lof* findNewOneColumn(Aircraft* aircraft);
	void edgeProcessFltFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessFltMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessMaintFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessMaintMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);

	void edgeProcessFlt(Leg* nextLeg, Aircraft* aircraft);	// ��ʼflt�趨
	void edgeProcessMaint(Leg* nextLeg, Aircraft* aircraft);	// ��ʼmaint�趨

	time_t computeFlightDelay (Leg* thisLeg, Leg* nextLeg); // ��nextLeg��flightʱ������delay, ����airport closure
	time_t delayByAirportClose (Leg* nextLeg, time_t delay); // ����flight, �Լ��趨Ҫ���ϵ�sequential delay������airport closure��ɵ�delay

	vector<Lof* > solveColGen();
	vector<Lof* > findInitColumns();
	void populateByColumn(vector<Lof* > _initColumns);
	void solve();
	void addColumns(vector<Lof* > _betterColumns);	//*
	vector<Lof* > solveIP();
};

#endif