#include"Aircraft.h"
//* #include"Lof.h"
//* #include"AircraftNode.h"

Aircraft::Aircraft(string name, time_t startT, time_t endT, Station * depS, Station * arrS)
	:_tail(name),_depStation(depS),_arrStation(arrS), _startT(startT), _endT(endT)
{
	_id = _count;
	_count++;
	_dual = 0;	//* ��Ӧmaster problemÿ�ܷɻ�ֻѡһ��Lof��constraint
}

int Aircraft::_count = 0;

void Aircraft::print()
{
	//* time_t startT = _startT - TIMEDIFF;
	//* time_t endT = _endT - TIMEDIFF;
	time_t startT = _startT;
	time_t endT = _endT;
	cout <<"Tail " << _tail <<  endl;
	cout << _depStation->getName() << " " << ctime(&startT);
	cout << _arrStation->getName() << " " << ctime(&endT); 
}

void Aircraft::sortScheLegByDepTime()
{
	sort(_planLegList.begin(), _planLegList.end(), Leg::compareDepTime);
}

bool Aircraft::isPlanLegFeasible()
{
	// �����ǰ�����һ��leg�Ƿ�����aircraft start/end time
	if (_planLegList.front()->getDepTime() < _startT)
		return false;
	if (_planLegList.back()->getArrTime() > _endT)
		return false;

	// �����ǰ�����һ��leg�Ƿ�����aircraft start/end airport
	if (_planLegList.front()->getDepStation() != _depStation)
		return false;
	if (_planLegList.back()->getArrStation() != _arrStation)
		return false;

	Leg* thisLeg = NULL;
	Leg* nextLeg = NULL;
	cout << "planLegList size: " << _planLegList.size() << endl;
	// ���airport match, time mismatch
	for (int i = 0; i < _planLegList.size() - 1; i++)
	{
		thisLeg = _planLegList[i];
		nextLeg = _planLegList[i+1];
		if (thisLeg->getArrStation() != nextLeg->getDepStation())
			return false;

		if (!thisLeg->isMaint() && !nextLeg->isMaint())		// flight, flight
		{
			if (thisLeg->getArrTime() + Util::turnTime > nextLeg->getDepTime())
				return false;
		}
		else// flight, maint; maint, flight; maint, maint
		{
			if (thisLeg->getArrTime() > nextLeg->getDepTime())
				return false;
		}
	}
	
	// ���maintenance��aircraft�Ƿ�match; dep time��arr time�Ƿ��ܵ�airport closureӰ��
	vector<pair<time_t, time_t>> depCloseList;
	vector<pair<time_t, time_t>> arrCloseList;
	for (int i = 0; i < _planLegList.size(); i++)
	{
		thisLeg = _planLegList[i];
		if (thisLeg->isMaint())
		{
			if (thisLeg->getAircraft() != this)
				return false;
		}
		else // flight check if affected by airport closure
		{
			depCloseList = thisLeg->getDepStation()->getCloseTimeList();
			for (int k = 0; k < depCloseList.size(); k++)
			{
				if (thisLeg->getDepTime() >= depCloseList[k].first &&
					thisLeg->getDepTime() < depCloseList[k].second)
					return false;
			}

			arrCloseList = thisLeg->getArrStation()->getCloseTimeList();
			for (int k = 0; k < arrCloseList.size(); k++)
			{
				if (thisLeg->getArrTime() >= arrCloseList[k].first &&
					thisLeg->getArrTime() < arrCloseList[k].second)
					return false;
			}
		}
	}

	return true;
}

/*ʡ����ʣ�µĳ�Ա����*/