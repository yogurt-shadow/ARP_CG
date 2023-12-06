#include "Schedule.h"

Schedule::Schedule(vector<Station* > stationList, vector<Aircraft* > aircraftList, vector<Leg* > legList)
	:_stationList(stationList), _aircraftList(aircraftList), _legList(legList)
{
	for(int i = 0; i < legList.size(); i++)
	{
		if (legList[i]->isMaint())
		{
			_maintList.push_back(legList[i]);
		} else
		{
			_flightList.push_back(legList[i]);
		}
	}

	cout <<"****** Total Number of Airports is " << _stationList.size()  << " ******"<< endl;
	for(int i = 0; i < _stationList.size(); i++)
	{
		_stationList[i]->print();
	}
	cout <<"****** Total Number of Aircraft is " << _aircraftList.size() << " ******"  << endl;
	for(int i = 0; i < _aircraftList.size(); i++)
	{
		_aircraftList[i]->print();
	}
	cout <<"******* Total Number of Maints is " << _maintList.size() << " *******" << endl;
	for(int i = 0; i < _maintList.size(); i++)
	{
		_maintList[i]->print();
	}

	cout <<"******* Total Number of Flights is " << _flightList.size() << " ******* " << endl;
	for(int i = 0; i < _flightList.size(); i++)
	{
		_flightList[i]->print();
	}

	setAdjascentLeg();
}

void Schedule::setAdjascentLeg()
{
	//* do it yourself
	//* 遍历所有的leg pair (sequence matters)
	//* 如果可以做neighbor (airport match, time match; 也要考虑maintenance的情况)
		//* push previous
		//* push next

	for (int i = 0; i < _legList.size(); i++)
	{
		for (int j = 0; j < _legList.size(); j++)
		{
			if (_legList[i]->isMaint() && _legList[j]->isMaint())	// both i and j are maintenance
			{
				if (_legList[i]->getArrStation() == _legList[j]->getDepStation()
					&& _legList[i]->getArrTime() <= _legList[j]->getDepTime()	// maintenance不可能delay
					//&& _legList[i]->getAircraft() == _legList[i]->getAircraft())	//BUG found here, corrected 20161226
					&& _legList[i]->getAircraft() == _legList[j]->getAircraft())
				{
					_legList[i]->pushNextLeg(_legList[j]);
					_legList[j]->pushPrevLeg(_legList[i]);
				}
			}

			if (_legList[i]->isMaint() && !_legList[j]->isMaint())	// i is maintenance, j is flight
			{
				if (_legList[i]->getArrStation() == _legList[j]->getDepStation()
					&& _legList[i]->getDepTime() <= _legList[j]->getDepTime()	//* flight是可以被delay的, 只要求flight的开始时间在maint开始时间之后
					&& _legList[i]->getArrTime() - _legList[j]->getDepTime() <= Util::maxDelayTime)		//* flight是可以被delay的, 只要求flight的开始时间在maint开始时间之后
				{
					_legList[i]->pushNextLeg(_legList[j]);
					_legList[j]->pushPrevLeg(_legList[i]);
				}
			}

			if (!_legList[i]->isMaint() && _legList[j]->isMaint())	// i is flight, j is maintenance
			{
				if (_legList[i]->getArrStation() == _legList[j]->getDepStation()
					&& _legList[i]->getArrTime() <= _legList[j]->getDepTime())	// maintenance不可能delay
				{
					_legList[i]->pushNextLeg(_legList[j]);
					_legList[j]->pushPrevLeg(_legList[i]);
				}
			}

			if (!_legList[i]->isMaint() && !_legList[j]->isMaint())	// both i and j are flight
			{
				if (_legList[i]->getArrStation() == _legList[j]->getDepStation()
					//&& _legList[i]->getDepTime() <= _legList[j]->getDepTime()	//*
					&& _legList[i]->getArrTime() - _legList[j]->getDepTime() <= Util::maxDelayTime)
				{
					if (_legList[i]->getDepTime() < _legList[j]->getDepTime())
					{
						_legList[i]->pushNextLeg(_legList[j]);
						_legList[j]->pushPrevLeg(_legList[i]);
					}

					if (_legList[i]->getDepTime() == _legList[j]->getDepTime())
					{
						if (_legList[i]->getId() < _legList[j]->getId()) //* use id to break tie
						{
							_legList[i]->pushNextLeg(_legList[j]);
							_legList[j]->pushPrevLeg(_legList[i]);
						}
					}
				}
			}
		}
	}

	//* compute total number of connections
	_connectionSize = 0;
	for (int i = 0; i < _legList.size(); i++)
	{
		_connectionSize += _legList[i]->getNextLegList().size();
	}

	cout << "############## TOTAL CONNECTION " << _connectionSize << " ###############" << endl;

	/*
	cout <<"############## PRINTING CONNECTING LEGS ############### " << endl;
	for(int i = 0; i < _legList.size(); i++)
	{
		_legList[i]->print();
		cout <<"Connect with Legs: " << endl;
		vector<Leg *> nextLegs = _legList[i]->getNextLegList();

		for(int j = 0; j < nextLegs.size(); j++)
		{
			nextLegs[j]->print();
		}
		cout <<"************************************* " << endl;
	}
	cout <<"############ END PRINTING CONNECTING LEGS ############# " << endl;
	*/
}

void Schedule::computeTopOrder()
{
	for (int i = 0; i < _legList.size(); i++)
	{
		if (!_legList[i]->isVisited())
		{
			dfs(_legList[i]);
		}
	}

	while (_reversePost.size() > 0)
	{
		_topOrderList.push_back(_reversePost.top());
		_reversePost.pop();
	}
}

void Schedule::dfs(Leg* leg)
{
	leg->setIsVisited(true);
	vector<Leg* > nextLegList = leg->getNextLegList();
	for ( int i = 0; i < nextLegList.size(); i++ )
	{
		if (!nextLegList[i]->isVisited())
		{
			dfs(nextLegList[i]);
		}
	}
	_reversePost.push(leg);
}