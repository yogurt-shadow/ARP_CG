#include"Station.h"
//* #include"AircraftNode.h"

Station::Station(string name)
	:_name(name)
{
	_id = _count;
	_count++;
	//* _terminateAircraftCount = 0;

	//* _dual.first = 0;
	//* _dual.second = 0;
}

int Station::_count = 0;

/*
void Station::setLegPro()
{
	_arrLegPro = _arrLegList.size()/_legNum;
	_depLegPro = _depLegList.size()/_legNum;
}
*/

void Station::print()
{
	cout <<"Station " << _name << " Id " << _id << endl;
	
	for(int i = 0; i < _closeTimeList.size(); i++)
	{
		time_t begin = _closeTimeList[i].first - TIMEDIFF;
		time_t end = _closeTimeList[i].second - TIMEDIFF;
		cout << "Bgn Close " << ctime(&begin);
		cout << "End Close " << ctime(&end);
	}
	
}

/*
void Station::sortNode()
{
	sort(_nodeList.begin(), _nodeList.end(), AircraftNode::compareTime);
}


void Station::printNodes()
{
	for(int i = 0; i < _nodeList.size(); i++)
	{
		_nodeList[i]->print();
	}
}

int Station::getDate()
{
	time_t early = _StartTime - DATECUTTIME;
	return early/DATEDURATION;
}
*/