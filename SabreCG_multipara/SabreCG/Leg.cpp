#include"Leg.h"


Leg::Leg(string flightNum)
	:_flightNum(flightNum),_depStation(NULL), _arrStation(NULL), _depTime(0), _arrTime(0), _aircraft(NULL)
{
	_id = -1;										//* dummyNode的id记为-1
}

Leg::Leg(string flightNum, Station * depStation, Station * arrStation, time_t depTime, time_t arrTime, Aircraft * aircraft)
	:_flightNum(flightNum),_depStation(depStation), _arrStation(arrStation), _depTime(depTime), _arrTime(arrTime), _aircraft(aircraft)
{
	_id = _count;
	_count++;

	_isVisited = false;			//*

	_aircraft->pushPlanLeg(this);					//* flight和maint都push在planLegList里

	if ( depStation != arrStation)                   // ordinary flight
	{
		_isMaint = false;
		//* flight和maint都push在planLegList里
		//* _aircraft->pushPlanLeg(this);               // Initialize the PlanLegList assigned to aircraft by schedule——aircrft
		_depStation->pushDepLeg(this);              // Initialize _depLegList depart from the station——station
		_arrStation->pushArrLeg(this);              // Initialize _arrLegList arriva at the station——station
	} else// maint
	{
		_isMaint = true;
		//* _aircraft->pushMaint(this);                 // Initialize the MaintList the aircraft needs carry——aircraft
		_depStation->pushMaint(this);               // Initialize the MainList carried at the station——station
	}
	_dual = 0;
	//* _lp = 0;
	_isAssigned = false;

	//_parent = NULL;				//*
	//* _delay = 0;				//*
	//_nodeCost = DBL_MAX;		//* DBL_MAX在<cfloat>定义，Maximum finite representable double number

	//_operDepTime = _depTime;	//* _operDepTime默认等于scheduled _depTime
	//_operArrTime = _arrTime;	//* _operArrTime默认等于scheduled _arrTime

	//_accDual = 0;				//*
	//_accDelay = 0;				//*

	//_delay = 0;					//*

	//_topOrderPrev = NULL;		//*
	//_topOrderNext = NULL;		//*
}

int Leg::_count = 0;

void Leg::print()
{
	//* time_t depTime = _depTime - TIMEDIFF; //*不用设置time difference了
	//* time_t arrTime = _arrTime - TIMEDIFF;
	time_t depTime = _depTime;
	time_t arrTime = _arrTime;
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

	// 打印multi label DP中subNodeList的信息
	/*
	cout << endl;
	cout << "total number of subnodes is " << _subNodeList.size() << endl;
	for(int i = 0; i < _subNodeList.size(); i++)
		_subNodeList[i]->print();
	cout << endl;
	*/

}

//bool Leg::resetLeg()	//* 重置Leg的_nodeCost, _operDepTime, _operArrTime, _parent, _accDual, _accDelay
bool Leg::resetLeg(int threadIndex)
{
	bool flag = false;
	//_nodeCost = DBL_MAX;
	//_operDepTime = _depTime;
	//_operArrTime = _arrTime;
	//_parent = NULL;

	//_accDual = 0;	//* for debug
	//_accDelay = 0;
	//_delay = 0;

	int sizeBefore = _subNodeList[threadIndex].size();
	// for(int i = 0; i < _subNodeList.size(); i++) //* BUG 在删除vector元素的过程中subNodeList的长度会不断改变 20170111 ##因为一边pop一边size就减小，直接用.clear();
	for(int i = 0; i < sizeBefore; i++)
	{
		popSubNode(threadIndex);
	}

	flag = true;
	return flag;
}

bool Leg::insertSubNode(SubNode* subNode, int threadIndex)
{
	//bool isInserted = false;

	// 如果_subNodeList是空的
	if (_subNodeList[threadIndex].empty())
	{
		_subNodeList[threadIndex].push_back(subNode);
		return true;
	}

	vector<SubNode* >::iterator itr;
	for (itr = _subNodeList[threadIndex].begin(); itr != _subNodeList[threadIndex].end(); )
	{
		if (SubNode::lessSubNodePointer(*itr, subNode)) // 如果新的subNode被已有的subNode dominate
		{
			return false;
		}

		if (SubNode::lessSubNodePointer(subNode, *itr)) // 如果新的subNode dominate已有的subNode
		{
			delete *itr;
			itr = _subNodeList[threadIndex].erase(itr);
			//isInserted = true;
		}
		else
		{
			itr++;
		}
	}

	// 如果程序走到这里，说明newSubNode没有被任何已有的subNode dominate
	_subNodeList[threadIndex].push_back(subNode);

	//cout << "leg:" << subNode->getLeg()->getId() << "insert subNode:" << endl;
	//subNode->print();
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

