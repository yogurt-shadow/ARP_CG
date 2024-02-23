#include "OperLeg.h"

OperLeg::OperLeg(Leg * leg)
	:_leg(leg)
{
	_depTime = _leg->getDepTime();
	_arrTime = _leg->getArrTime();
	_operAircraft = NULL;
}

//*  用在subproblem生成LoF时，直接指定aircraft
OperLeg::OperLeg(Leg * leg, Aircraft * aircraft)
	:_leg(leg)
{
	_depTime = _leg->getDepTime();
	_arrTime = _leg->getArrTime();
	_operAircraft = aircraft;
}

/*
void OperLeg::delayTo(time_t time)
{
	if ( time < _depTime)
	{
		cout <<"Error is OperLeg::delayTo " << time << endl;
		exit(0);
	}

	time_t _duration = 0;
	_duration = _arrTime - _depTime;
	_arrTime = time + _duration;
	_depTime = time;
}
*/

/*
time_t OperLeg::getOpDepTime() 
{
	if ( _leg->isMaint() ) /* 为什么要在maintenance的depTime上加turnTime?
	{
		return _depTime + Util::turnTime;
	}
	return _depTime;
}
*/

time_t OperLeg::getOpDepTime() 
{
	return _depTime;
}

void OperLeg::print()
{
	if (_operAircraft != NULL)
	{
		_operAircraft->print();
	}
	_leg->print();
	//* time_t depTime = _depTime - TIMEDIFF;
	//* time_t arrTime = _arrTime - TIMEDIFF;
	time_t depTime = _depTime;
	time_t arrTime = _arrTime;
	cout <<"ODp " << ctime(&depTime);
	cout <<"OAr " << ctime(&arrTime) << endl;
}

/*
OperLeg * OperLeg::clone()
{
	OperLeg * newOperLeg = new OperLeg(_leg);
	newOperLeg->setOpDepTime(_depTime);
	newOperLeg->setOpArrTime(_arrTime);
	newOperLeg->setOpAircraft(_operAircraft);
	return newOperLeg;
}
*/

/* process delay according to airport closure by leg */
/*
time_t OperLeg::delayByAirportClose()
{
	// if leg is maint, cannot affected by airport closure
	if ( _leg->isMaint() )
	{
		return 0;
	}
	Station * depStation = _leg->getDepStation();
	Station * arrStation = _leg->getArrStation();

	vector<pair<time_t, time_t> > closeTimeList = depStation->getCloseTimeList();
	
	// departure time within the closure time
	for(int i = 0; i < closeTimeList.size(); i++)
	{
		if ( _depTime >= closeTimeList[i].first && 
			_depTime < closeTimeList[i].second )
		{
			return closeTimeList[i].second - _depTime;		/// 不考虑dep delay加上后 arrival time落在arrival station的closure interval的情况
		}
	}

	closeTimeList = arrStation->getCloseTimeList();			
	for(int i = 0; i < closeTimeList.size(); i++)
	{
		if ( _arrTime >= closeTimeList[i].first && 
			_arrTime < closeTimeList[i].second )
		{
			return closeTimeList[i].second - _arrTime;
		}
	}

	return 0;
}
*/