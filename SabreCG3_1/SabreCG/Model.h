// Model for Multi-label DP 20170108
#ifndef MODEL_H
#define MODEL_H

#include "Util.h"
#include "Leg.h"
#include "Aircraft.h"
#include "Station.h"
#include "Lof.h"
#include "OperLeg.h"

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
	vector<Lof* > _finalLofList;			// final selected lofs
	vector<Leg* > _cancelLegList;			// cancelled flights/maint

	static int _count;						// column generation�ĵ�������
	
	static string header;
	vector<Leg *> _legList;                 // flight + maint
	vector<Aircraft *> _aircraftList;		// aircraft
	vector<Station *> _stationList;			// airport

	vector<Lof* > _initColumns;				//* ��ʼcolumn, ��findInitColumns��Ա������ʼ

	vector<Leg *> _topOrderList;			//* leg topological order

	bool fileExist(string fileName);
	//Leg _dummySource;						//* dummy source node connecting starting nodes // ʹ��findNewOneColumn��debug�ȽϷ���
public:
	Model(vector<Station *> stationList, vector<Aircraft *> aircraftList, vector<Leg *> legList, vector<Leg *> topOrderList);
	vector<Lof *> findNewColumns();
	vector<Lof *> findNewMultiColumns(Aircraft* aircraft, int);

	Lof* findNewOneColumn(Aircraft* aircraft);
	void edgeProcessFltFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessFltFltSubNode(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft);

	void edgeProcessFltMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessFltMaintSubNode(SubNode* thisLeg, Leg* nextLeg, Aircraft* aircraft);

	void edgeProcessMaintFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessMaintFltSubNode(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft);

	void edgeProcessMaintMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft);
	void edgeProcessMaintMaintSubNode(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft);

	void edgeProcessFlt(Leg* nextLeg, Aircraft* aircraft, int);
	void edgeProcessMaint(Leg* nextLeg, Aircraft* aircraft);

	time_t computeFlightDelay (SubNode* subNode, Leg* nextLeg);
	time_t delayByAirportClose (Leg* nextLeg, time_t delay);

	vector<Lof* > solveColGen();
	vector<Lof* > findInitColumns();
	Lof* findInitOneColumn(Aircraft* aircraft);
	void populateByColumn(vector<Lof* > _initColumns);
	void solve();
	void addColumns(vector<Lof* > _betterColumns);	//*
	vector<Lof* > solveIP();
	void print();
};

#endif