#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "Util.h"
#include "Leg.h"
#include "Aircraft.h"
#include "Station.h"

class Schedule
{
private:

	vector<Leg* > _legList;			// flights + maintenance
	vector<Aircraft* > _aircraftList;
	vector<Station* > _stationList;
	vector<Leg *> _maintList;				// only maints
	vector<Leg *> _flightList;				// only legs

	vector<Leg* > _topOrderList;		//* topological order list
	stack<Leg* > _reversePost;			//* helper for finding topological order, save legs during dfs

	void dfs(Leg* leg);					//* helper function for finding topological order

	int _connectionSize;				//* used to save total number of connections

public:

	Schedule(vector<Station* > stationList, vector<Aircraft* > aircraftList, vector<Leg* > legList);
	void setAdjascentLeg();

	void computeTopOrder();
	vector<Leg* > getTopOrderList() {return _topOrderList;}

	int getConnectionSize() {return _connectionSize;}
};

#endif