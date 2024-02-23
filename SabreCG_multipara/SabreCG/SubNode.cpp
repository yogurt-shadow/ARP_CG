#include "SubNode.h"
#include "Leg.h"

SubNode::SubNode(Leg* leg = NULL, SubNode* parentSubNode = NULL, double subNodeCost = 0, time_t delay = 0):
	_leg(leg), _parentSubNode(parentSubNode), _subNodeCost(subNodeCost), _delay(delay)
{
}

void SubNode::print()
{
	cout << "subNodeCost is   " << _subNodeCost << endl;
	cout << "subNode delay is " << _delay << endl;

	if (_leg != NULL)
		cout << "hosting leg is lg" << _leg->getId() << endl;
	else
		cout << "hosting leg is NULL" << endl;

	if (_parentSubNode != NULL)
		cout << "parent subnode's hosting leg is lg" << _parentSubNode->getLeg()->getId() << endl;
	else
		cout << "parent subnode is NULL" << endl;

}

time_t SubNode::getOperDepTime()
{
	return _leg->getDepTime() + _delay;
}

time_t SubNode::getOperArrTime()
{
	return _leg->getArrTime() + _delay;
}

bool SubNode::lessSubNodePointer(SubNode* p1, SubNode* p2)
{
	if (p1->getSubNodeCost() < p2->getSubNodeCost()
		&& p1->getDelay() < p2->getDelay())
		return true;

	if (p1->getSubNodeCost() <= p2->getSubNodeCost()
		&& p1->getDelay() < p2->getDelay())
		return true;

	if (p1->getSubNodeCost() < p2->getSubNodeCost()
		&& p1->getDelay() <= p2->getDelay())
		return true;

	return false;
}

bool SubNode::cmpByCost(SubNode * a, SubNode * b)
{
	return a->getSubNodeCost() < b->getSubNodeCost();
}
