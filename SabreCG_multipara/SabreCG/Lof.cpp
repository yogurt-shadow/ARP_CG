#include "Lof.h"

Lof::Lof()
{
	_aircraft = NULL;
	_cost = 0;
	//* _purity = 0;
	//* _id = 0;
	_id = _count;			//*
	_count++;				//*
}

void Lof::pushLeg(OperLeg * leg)
{
	_legList.push_back(leg);

	if( leg->getLeg()->isMaint() )
	{
		_maintList.push_back(leg);
	}
}

int Lof::_count = 0;		//*

/*
//* duplicate Lof and assign aircraft
Lof * Lof::clone()
{
	Lof * lof = new Lof();

	for(int i = 0; i < _legList.size(); i++)
	{
		OperLeg * newLeg = _legList[i]->clone();     // ScheduledLeg + depTime + arrTime + OpeAircraft
		lof->pushLeg(newLeg);
	}

	//	lof->setLegList(_legList);
	lof->setAircraft(_aircraft);
	lof->setCost(_cost);
	lof->setPurity(_purity);
	return lof;
}
*/

Lof::~Lof()
{
	for(int i =0; i < _legList.size(); i++)
	{
		delete _legList[i];
	}
}

void Lof::print()
{
	cout <<"************ LOF ************" << endl;
	cout<< "Lof ID is " << _id <<endl;
	if ( _aircraft != NULL)
	{
		_aircraft->print();
	}
	else
	{
		cout << "Tail is NULL" << endl; //*
	}
	cout <<"Leg List Size is " << _legList.size() << " cost " << _cost <<  endl << endl;
	//	cout <<"Leg List Size is " << _legList.size() << " purity " << _purity <<  endl << endl;

	for(int i = 0; i < _legList.size(); i++)
	{
		_legList[i]->print();
	}

/*	if (_depNode!=NULL)
	{
		cout<<"ID of depNode "<<_depNode->getId()<<endl;
			cout<<"departure station is "<<_depNode->getStation()->getName()<<endl;
		if (_depNode->getAirctaft() != NULL)
		{
		cout<<"aircraft is "<<_depNode->getAirctaft()->getTail()<<endl;
		}
		time_t depTime = _depNode->getTime() - TIMEDIFF;
		cout<<"Node time is "<< ctime(&depTime)<<endl;
	/*	if (_depNode->getNextNode()!=NULL)
		{
		cout<<"ID of next node "<<_depNode->getNextNode()->getId()<<endl;
		cout<<"station of next node is "<<_depNode->getNextNode()->getStation()->getName()<<endl;
		cout<<"aircraft is "<<_depNode->getNextNode()->getAirctaft()->getTail()<<endl;
		time_t Time = _depNode->getNextNode()->getTime() - TIMEDIFF;
		cout<<"departure time is "<< ctime(&Time)<<endl;
		}*/
//	}
/*	if (_arrNode!=NULL)
	{
		cout<<"ID of arrNode "<<_arrNode->getId()<<endl;
			cout<<"arrival station is "<<_arrNode->getStation()->getName()<<endl;
		if (_arrNode->getAirctaft() != NULL)
		{
		cout<<"aircraft is "<< _arrNode->getAirctaft()->getTail()<<endl;
		}
		time_t arrTime = _arrNode->getTime() - TIMEDIFF;
		cout<<"Node time is "<< ctime(&arrTime)<<endl;
	/*	if (_arrNode->getNextNode()!=NULL)
		{
		cout<<"ID of next node "<<_arrNode->getNextNode()->getId()<<endl;
		cout<<"station of next node is "<<_arrNode->getNextNode()->getStation()->getName()<<endl;
		cout<<"aircraft is "<<_arrNode->getNextNode()->getAirctaft()->getTail()<<endl;
		time_t Time = _arrNode->getNextNode()->getTime() - TIMEDIFF;
		cout<<"arrival time is "<< ctime(&Time)<<endl;
		}*/

//	}
	cout <<"*****************************"  << endl;

}

/*
void Lof::compPurity()
{
	vector<Aircraft *> usedAircraftList;
	for(int i = 0; i < _legList.size(); i++)
	{
		bool found = false;
		for(int j = 0; j < usedAircraftList.size(); j++)
		{
			if ( _legList[i]->getLeg()->getAircraft() == usedAircraftList[j])
			{
				found = true;
				break;
			}
		}

		if ( found == false)
		{
			usedAircraftList.push_back( _legList[i]->getLeg()->getAircraft());
		}
	}
	_purity = usedAircraftList.size();
}
*/

/*
bool Lof::compatible(Aircraft * aircraft)
*/

/*
	bool rescheduleTime(time_t t);                         /// ????????????????????????????????????? 都是处理sequential delay的?
	bool rescheduleTime();
	bool rescheduleTimeWithMaint();
	bool rescheduleTimeWithoutMaint();
	bool rescheduleTimeWithMaint(time_t prevEndTime);
	bool rescheduleTimeWithoutMaint(time_t prevEndTime);
	void rescheduleTimeWithoutMaintAirportClose();
	void rescheduleTimeWithMaintAirportClose();
	*/

/* compute the cost of the lof = delay + swap */
/*
void Lof::compCostWithoutPopulate()
{
	_cost = 0;
	for(int i = 0; i < _legList.size(); i++)
	{
		if ( !_legList[i]->getLeg()->isMaint() )
		{
			_cost = _cost + (_legList[i]->getOpDepTime() - _legList[i]->getScheDepTime())/60 * Util::w_fltDelay;
		}
	}
	compPurity();
	_cost = _cost + _purity * Util::w_fltSwap;
}
*/

void Lof::computeLofCost()
{
	_cost = 0;
	for(int i = 0; i < _legList.size(); i++)
	{
		if ( !_legList[i]->getLeg()->isMaint() )
		{
			_cost = _cost + (_legList[i]->getOpDepTime() - _legList[i]->getScheDepTime())/60.0 * Util::w_fltDelay; /* 注意这里除以60 */
			//_cost = _cost + (_legList[i]->getOpDepTime() - _legList[i]->getScheDepTime()) * Util::w_fltDelay; // 小case debug用
		}

		if ( _legList[i]->getScheAircraft() != _aircraft)
		{
			_cost = _cost + Util::w_fltSwap;
		}
	}
}

time_t Lof::getOperArrTime()
{ // with turn time added, can depart immediately...
	if ( _legList.back()->getLeg()->isMaint() )
	{
		return _legList.back()->getOpArrTime();
	}
	//* return _legList.back()->getOpArrTime() + Util::turnTime;
	return _legList.back()->getOpArrTime();
}

time_t Lof::getOperDepTime()
{ // with turn time deducted if maint, can depart immediately
	/*	if ( _legList.front()->getLeg()->isMaint())
	{
	return _legList.front()->getOpDepTime() + Util::turnTime;
	}
	*/
	return _legList.front()->getOpDepTime();
}

time_t Lof::getDepTime()
{
	return _legList.front()->getPrintDepTime();
}

bool Lof::compareDepTime(Lof * a, Lof *b)
{
	if(a->getOperDepTime() < b->getOperDepTime() )
	{
		return true;
	}
	return false;
}

bool Lof::compareByAircraft(Lof* a, Lof* b)
{
	if (a->getAircraft()->getId() < b->getAircraft()->getId())	// 比较aircraft
	{
		return true;
	}

	if (a->getAircraft()->getId() == b->getAircraft()->getId())	// aircraft相同，比较lof id
	{
		return a->getId() < b->getId();
	}

	return false;

}

void Lof::computeReducedCost()
{
	_reducedCost = 0;
	double sumLegDual = 0;		//*

	/* flow balance constraint */
	//* _reducedCost = _depNode->getDual() - _arrNode->getDual();     ///********注意depNode是正，arrNode是负 /// flow balance constraint的dual是什么时候被set的？

	/* cover constraint */

	//*cout << "_legList.size() is " <<  _legList.size() << endl;

	for (int i = 0; i < _legList.size(); i++)
	{
		//* _reducedCost += _legList[i]->getLeg()->getDual();
		sumLegDual += _legList[i]->getLeg()->getDual();
	}

	//* _reducedCost = _reducedCost - getCost();

	_reducedCost = _cost - sumLegDual - _aircraft->getDual();

}