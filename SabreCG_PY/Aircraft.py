from Station import Station
from Leg import Leg
from typing import List

class Aircraft:
    _count = 0
    """
    _tail: Tail Number
    _id
    _count
    _startT
    _endT
    _depStation
    _arrStation
    _dual
    _planLegList
    """
    def __init__(self, name: str, startT: float, endT: float, depS: Station, arrS: Station):
        self._planLegList = []
        self._tail = name
        self._startT, self._endT = startT, endT
        self._depStation, self._arrStation = depS, arrS
        self._id = _count
        _count += 1
        self._dual = 0
    
    def print(self) -> None:
        startT, endT = self._startT, self._endT
        print("Tail %s" % self._tail)
        print("%s %s" % (self._depStation.getName(), startT))
        print("%s %s" % (self._arrStation.getName(), endT))

    def getTail(self) -> str:
        return self._tail
    
    def pushPlanLeg(self, leg: Leg) -> None:
        self._planLegList.append(leg)

    def getDepStation(self) -> Station:
        return self._depStation
    
    def getArrStation(self) -> Station:
        return self._arrStation
    
    def getStartTime(self) -> float:
        return self._startT
    
    def getEndTime(self) -> float:
        return self._endT
    
    def getId(self) -> int:
        return self._id
    
    def setDual(self, dual: float) -> None:
        self._dual = dual

    def getDual(self) -> float:
        return self._dual
    
    def getPlanLegList(self) -> List[Leg]:
        return self._planLegList

    def sortScheLegByDepTime(self):
        self._planLegList.sort(key = lambda _leg: _leg.compareDepKey)

    def isPlanLegFeasible(self):
        pass