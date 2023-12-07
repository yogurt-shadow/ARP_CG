import util as ut
from Station import Station
from Aircraft import Aircraft
from SubNode import SubNode
from typing import List

class Leg:
    _count = 0

    def __init__(self, flightNum: str, depStation: Station, arrStation: Station, depTime: float, arrTime: float, aircraft: Aircraft):
        self._flightNum = flightNum
        self._depStation, self._arrStation = depStation, arrStation
        self._depTime, self._arrTime = depTime, arrTime
        self._aircraft = aircraft
        self._id = _count
        _count += 1
        self._isVisited = False
        self._aircraft.pushPlanLeg(self)
        if depStation != arrStation: # flight
            self._isMaint = False
            self._depStation.pushDepLeg(self)
            self._arrStation.pushArrLeg(self)
        else: # maintaince
            self._isMaint = True
            self._depStation.pushMaint(self)
        self._dual = 0
        self._isAssigned = False

    @classmethod
    def initFlighytNum(self, flightNum: str) -> 'Leg':
        self._flightNum = flightNum
        self._depStation, self._arrStation = None, None
        self._depTime, self._arrTime = 0, 0
        self._aircraft = None
        self._id = -1
        return self
    
    def print(self) -> None:
        depTime = self._depTime - ut.TIMEDIFF
        arrTime = self._arrTime - ut.TIMEDIFF
        if not self._isMaint:
            print("Leg %d Flt %s Tal %s" % (self._id, self._flightNum, self._aircraft.getTail()))
            print("%s %s" % (self._depStation.getName(), depTime))
            print("%s %s" % (self._arrStation, arrTime))
        else:
            print("Maint %d Flt %s Sta %s Tal %s" % (self._id, self._flightNum, self._depStation.getName(), self._aircraft.getTail()))
            print(depTime)
            print(arrTime)

    def setId(self, id: int) -> None:
        self._id = id

    def getId(self) -> int:
        return self._id
    
    def getFlightNum(self) -> str:
        return self._flightNum
    
    def getArrStation(self) -> Station:
        return self._arrStation
    
    def getDepStation(self) -> Station:
        return self._depStation
    
    def getArrTime(self) -> float:
        return self._arrTime
    
    def getDepTime(self) -> float:
        return self._depTime
    
    def getAircraft(self) -> Aircraft:
        return self._aircraft
    
    def setPrevLegList(self, legList: List['Leg']) -> None:
        self._prevLegList = legList

    def setNextLegList(self, legList: List['Leg']) -> None:
        self._nextLegList = legList
    
    def getPrevLegList(self) -> List['Leg']:
        return self._prevLegList
    
    def getNextLegList(self) -> List['Leg']:
        return self._nextLegList
    
    def pushNextLeg(self, leg: 'Leg') -> None:
        self._nextLegList.append(leg)

    def pushPrevLeg(self, leg: 'Leg') -> None:
        self._prevLegList.append(leg)

    def isMaint(self) -> bool:
        return self._isMaint
    
    def setIsMaint(self, b: bool) -> None:
        self._isMaint = b

    def isVisited(self) -> bool:
        return self._isVisited
    
    def setIsVisited(self, b: bool) -> None:
        self._isVisited = b

    def setDual(self, dual: float) -> None:
        self._dual = dual

    def getDual(self) -> float:
        return self._dual
    
    def setAssigned(self, b: bool) -> None:
        self._isAssigned = b

    def getAssigned(self) -> bool:
        return self._isAssigned

    def resetLeg(self) -> None:
        flag = False
        size = len(self._subNodeList)
        for i in range(size):
            self.popSubNode()
        flag = True
        return flag
    
    def getSubNodeList(self) -> List[SubNode]:
        return self._subNodeList
    
    def setSubList(self, subNodeList: List[SubNode]) -> None:
        self._subNodeList = subNodeList

    def pushSubNode(self, subNode: SubNode) -> None:
        self._subNodeList.append(subNode)

    def popSubNode(self) -> None:
        self._subNodeList.pop()

    def insertSubNode(self, subNode: SubNode) -> bool:
        if len(self._subNodeList) == 0:
            self._subNodeList.append(subNode)
            return True
        for _subNode in self._subNodeList:
            if SubNode.LessKey(_subNode, subNode):
                return False
            if SubNode.LessKey(subNode, _subNode):
                self._subNodeList.remove(_subNode)
        self._subNodeList.append(subNode)
        return True

    def compareDepKey(self) -> float:
        return self.getDepTime()
    