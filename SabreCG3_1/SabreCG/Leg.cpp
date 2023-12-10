#include"Leg.h"

Leg::Leg(string flightNum)
	:_flightNum(flightNum),_depStation(NULL), _arrStation(NULL), _depTime(0), _arrTime(0), _aircraft(NULL)
{
	_id = -1;										//* dummyNode��id��Ϊ-1
}

Leg::Leg(string flightNum, Station * depStation, Station * arrStation, time_t depTime, time_t arrTime, Aircraft * aircraft)
	:_flightNum(flightNum),_depStation(depStation), _arrStation(arrStation), _depTime(depTime), _arrTime(arrTime), _aircraft(aircraft)
{
	_id = _count;
	_count++;

	_isVisited = false;			//*

	_aircraft->pushPlanLeg(this);					//* flight��maint��push��planLegList��

	if ( depStation != arrStation)                   // ordinary flight
	{
		_isMaint = false;
		//* flight��maint��push��planLegList��
		//* _aircraft->pushPlanLeg(this);               // Initialize the PlanLegList assigned to aircraft by schedule����aircrft
		_depStation->pushDepLeg(this);              // Initialize _depLegList depart from the station����station
		_arrStation->pushArrLeg(this);              // Initialize _arrLegList arriva at the station����station
	} else// maint
	{
		_isMaint = true;
		//* _aircraft->pushMaint(this);                 // Initialize the MaintList the aircraft needs carry����aircraft
		_depStation->pushMaint(this);               // Initialize the MainList carried at the station����station
	}
	_dual = 0;
	//* _lp = 0;
	_isAssigned = false;

	//_parent = NULL;				//*
	//* _delay = 0;				//*
	//_nodeCost = DBL_MAX;		//* DBL_MAX��<cfloat>���壬Maximum finite representable double number

	//_operDepTime = _depTime;	//* _operDepTimeĬ�ϵ���scheduled _depTime
	//_operArrTime = _arrTime;	//* _operArrTimeĬ�ϵ���scheduled _arrTime

	//_accDual = 0;				//*
	//_accDelay = 0;				//*

	//_delay = 0;					//*

	//_topOrderPrev = NULL;		//*
	//_topOrderNext = NULL;		//*
}

int Leg::_count = 0;

void Leg::print()
{
	time_t depTime = _depTime - TIMEDIFF;
	time_t arrTime = _arrTime - TIMEDIFF;
	//time_t depTime = _depTime;
	//time_t arrTime = _arrTime;
	if ( !_isMaint)
	{
		cout <<"Leg " << _id << " Flt " << _flightNum << " Tal " << _aircraft->getTail() << endl;
		cout << _depStation->getName() << " " << ctime(&depTime);
		cout << _arrStation->getName() << " " << ctime(&arrTime);
	} else
	{
		cout <<"Maint " << _id << " Flt " << _flightNum <<  " Sta "<< _depStation->getName()  << " Tal " << _aircraft->getTail() <<  endl;
		cout << ctime(&depTime);
		cout << ctime(&arrTime);
	}

	// ��ӡmulti label DP��subNodeList����Ϣ
	/*
	cout << endl;
	cout << "total number of subnodes is " << _subNodeList.size() << endl;
	for(int i = 0; i < _subNodeList.size(); i++)
		_subNodeList[i]->print();
	cout << endl;
	*/

}

//bool Leg::resetLeg()	//* ����Leg��_nodeCost, _operDepTime, _operArrTime, _parent, _accDual, _accDelay
bool Leg::resetLeg()
{
	bool flag = false;
	//_nodeCost = DBL_MAX;
	//_operDepTime = _depTime;
	//_operArrTime = _arrTime;
	//_parent = NULL;

	//_accDual = 0;	//* for debug
	//_accDelay = 0;
	//_delay = 0;

	int sizeBefore = _subNodeList.size();
	// for(int i = 0; i < _subNodeList.size(); i++) //* BUG ��ɾ��vectorԪ�صĹ�����subNodeList�ĳ��Ȼ᲻�ϸı� 20170111
	for(int i = 0; i < sizeBefore; i++)
	{
		popSubNode();
	}

	flag = true;
	return flag;
}

bool Leg::insertSubNode(SubNode* subNode)
{
	//bool isInserted = false;

	// ���_subNodeList�ǿյ�
	if (_subNodeList.empty())
	{
		_subNodeList.push_back(subNode);
		return true;
	}

	vector<SubNode* >::iterator itr;
	for (itr = _subNodeList.begin(); itr != _subNodeList.end(); )
	{
		if (SubNode::lessSubNodePointer(*itr, subNode)) // ����µ�subNode�����е�subNode dominate
		{
			return false;
		}

		if (SubNode::lessSubNodePointer(subNode, *itr)) // ����µ�subNode dominate���е�subNode
		{
			delete *itr;
			itr = _subNodeList.erase(itr);
			//isInserted = true;
		}
		else
		{
			itr++;
		}
	}

	// ��������ߵ����˵��newSubNodeû�б��κ����е�subNode dominate
	_subNodeList.push_back(subNode);

	return true;
}

bool Leg::compareDepTime(Leg * a, Leg * b)
{
	if (a->getDepTime() < b->getDepTime())
	{
		return true;
	}
	return false;
}

/*
int Leg::getDate()
{
	time_t early = _depTime - DATECUTTIME;
//	cout << ctime(&early);
//	cout << asctime(&early);
//	cout << ((double) early)/DATEDURATION << endl;
	return early/DATEDURATION;
}

bool Leg::compareArrTime(Leg * a, Leg * b)// sort arrival time of legs
{
	if ( a->getArrTime() < b->getArrTime())
	{
		return true;
	}
	return false;
}
*/