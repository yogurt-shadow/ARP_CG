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
	cout <<"*****************************"  << endl;

}

void Lof::computeLofCost()
{
	_cost = 0;
	for(int i = 0; i < _legList.size(); i++)
	{
		if ( !_legList[i]->getLeg()->isMaint() )
		{
			_cost = _cost + (_legList[i]->getOpDepTime() - _legList[i]->getScheDepTime())/60.0 * Util::w_fltDelay; /* ע���������60 */
			//_cost = _cost + (_legList[i]->getOpDepTime() - _legList[i]->getScheDepTime()) * Util::w_fltDelay; // Сcase debug��
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

void Lof::computeReducedCost()
{
	_reducedCost = 0;
	double sumLegDual = 0;		//*

	/* flow balance constraint */
	//* _reducedCost = _depNode->getDual() - _arrNode->getDual();     ///********ע��depNode������arrNode�Ǹ� /// flow balance constraint��dual��ʲôʱ��set�ģ�

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