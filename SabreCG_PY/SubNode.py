from typing import List
from Leg import Leg

class SubNode:
    def __init__(self, leg: Leg = None, parentSubNode: 'SubNode' = None, subNodeCost: float = 0, delay: float = 0):
        self._leg = leg
        self._parentSubNode, self._subNodeCost, self._delay = parentSubNode, subNodeCost, delay

    def getLeg(self) -> Leg:
        return self._leg

    def setLeg(self, leg: Leg) -> None:
        self._leg = leg

    def getParentSubNode(self) -> 'SubNode':
        return self._parentSubNode
    
    def setParentSubNode(self, parentSubNode: 'SubNode') -> None:
        self._parentSubNode = parentSubNode

    def getSubNodeCost(self) -> float:
        return self._subNodeCost

    def setSubNodeCost(self, subNodeCost: float) -> None:
        self._subNodeCost = subNodeCost

    def getDelay(self) -> float:
        return self._delay
    
    def setDelay(self, delay: float) -> None:
        self._delay = delay

    def print(self) -> None:
        print("subNodeCost is " + str(self._subNodeCost))
        print("subNode delay is " + str(self._delay))
        if self._leg != None:
            print("hosting leg is lg" + str(self._leg.getId()))
        else:
            print("hosting leg is Null")
        if self._parentSubNode != None:
            print("parent subnode's hosting leg is lg" + str(self._parentSubNode.getLeg.getId()))
        else:
            print("parent subNode is Null")

    def getOperDepTime(self) -> float:
        return self._leg.getDepTime() + self._delay
    
    def getOperArrTime(self) -> float:
        return self._leg.getArrTime() + self._delay
    
    @classmethod
    def LessKey(p1, p2) -> bool:
        if p1.getSubNodeCost() == p2.getSubNodeCost() and p1.getDelay() == p2.getDelay():
            return False
        a, b = p1.getSubNodeCost() <= p2.getSubNodeCost(), p1.getDelay() <= p2.getDelay()
        return a and b

    def CostKey(self) -> float:
        return self.getSubNodeCost()