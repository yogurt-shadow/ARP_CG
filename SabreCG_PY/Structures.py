import Util as ut
from typing import List
from Stack import Stack
import time

class Station:
    count = 0
    def __init__(self, name: str):
        self._name = name
        self._id = Station.count
        Station.count += 1
        self._depLegList, self._arrLegList = [], []
        self._depAircraft = []
        self._maintList = []
        self._closeTimeList = []

    def getName(self) -> str:
        return self._name
    
    def pushDepLeg(self, leg) -> None:
        self._depLegList.append(leg)

    def pushArrLeg(self, leg) -> None:
        self._arrLegList.append(leg)

    def pushDepAircraft(self, _aircraft) -> None:
        self._depAircraft.append(_aircraft)

    def getDepAircraftNumber(self) -> int:
        return len(self._depAircraft)
    
    def getDepLegList(self):
        return self._depLegList
    
    def getArrLegList(self):
        return self._arrLegList
    
    def pushMaint(self, maint) -> None:
        self._maintList.append(maint)

    def getMainList(self):
        return self._maintList
    
    def pushCloseTime(self, closeTime: (float, float)) -> None:
        self._closeTimeList.append(closeTime)

    def getCloseTimeList(self) -> (float, float):
        return self._closeTimeList
    
    def print(self) -> None:
        print("Station %s Id %d" % (self._name, self._id))
        for _time in self._closeTimeList:
            begin, end = _time[0] - ut.TIMEDIFF, _time[1] - ut.TIMEDIFF
            print("Bgn Close", time.ctime(begin))
            print("End Close", time.ctime(end))

    def getId(self) -> int:
        return self._id
    
    def setLegNum(self, _size: int) -> None:
        self._legNum = _size

class SubNode:
    def __init__(self, leg = None, parentSubNode: 'SubNode' = None, subNodeCost: float = 0, delay: float = 0):
        self._leg = leg
        self._parentSubNode, self._subNodeCost, self._delay = parentSubNode, subNodeCost, delay

    def getLeg(self):
        return self._leg

    def setLeg(self, leg) -> None:
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
    
    def LessKey(self, other) -> bool:
        if self.getSubNodeCost() == other.getSubNodeCost() and self.getDelay() == other.getDelay():
            return False
        a, b = self.getSubNodeCost() <= other.getSubNodeCost(), self.getDelay() <= other.getDelay()
        return a and b

    def CostKey(self) -> float:
        return self.getSubNodeCost()

class Leg:
    _count = 0

    def __init__(self, flightNum: str, depStation: Station, arrStation: Station, depTime: float, arrTime: float, aircraft):
        self._flightNum = flightNum
        self._depStation, self._arrStation = depStation, arrStation
        self._depTime, self._arrTime = depTime, arrTime
        self._aircraft = aircraft
        self._id = Leg._count
        Leg._count += 1
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
        self._nextLegList, self._prevLegList = [], []
        self._subNodeList = []

    @classmethod
    def initFlighytNum(self, flightNum: str) -> 'Leg':
        self._flightNum = flightNum
        self._depStation, self._arrStation = None, None
        self._depTime, self._arrTime = 0, 0
        self._aircraft = None
        self._id = -1
        self._nextLegList, self._prevLegList = [], []
        self._subNodeList = []
        return self
    
    def print(self) -> None:
        depTime = self._depTime - ut.TIMEDIFF
        arrTime = self._arrTime - ut.TIMEDIFF
        if not self._isMaint:
            print("Leg %d Flt %s Tal %s" % (self._id, self._flightNum, self._aircraft.getTail()))
            print("%s %s" % (self._depStation.getName(), time.ctime(depTime)))
            print("%s %s" % (self._arrStation.getName(), time.ctime(arrTime)))
        else:
            print("Maint %d Flt %s Sta %s Tal %s" % (self._id, self._flightNum, self._depStation.getName(), self._aircraft.getTail()))
            print(time.ctime(depTime))
            print(time.ctime(arrTime))

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
    
    def getAircraft(self):
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
            if _subNode.LessKey(subNode):
                return False
            if subNode.LessKey(_subNode):
                self._subNodeList.remove(_subNode)
        self._subNodeList.append(subNode)
        return True

    def compareDepKey(self) -> float:
        return self.getDepTime()
    

class Aircraft:
    _count = 0
    def __init__(self, name: str, startT: float, endT: float, depS: Station, arrS: Station):
        self._planLegList = []
        self._tail = name
        self._startT, self._endT = startT, endT
        self._depStation, self._arrStation = depS, arrS
        self._id = Aircraft._count
        Aircraft._count += 1
        self._dual = 0
    
    def print(self) -> None:
        startT, endT = self._startT, self._endT
        print("Tail %s" % self._tail)
        print("%s %s" % (self._depStation.getName(), time.ctime(startT)))
        print("%s %s" % (self._arrStation.getName(), time.ctime(endT)))

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

    def sortScheLegByDepTime(self) -> None:
        self._planLegList.sort(key = lambda _leg: _leg.compareDepKey())

    def isPlanLegFeasible(self) -> bool:
        if self._planLegList[0].getDepTime() < self._startT:
            return False
        if self._planLegList[-1].getArrTime() > self._endT:
            return False
        if self._planLegList[0].getDepStation() != self._depStation:
            return False
        if self._planLegList[-1].getArrStation() != self._arrStation:
            return False
        thisLeg, nextLeg = None, None
        for i in range(len(self._planLegList) - 1):
            thisLeg, nextLeg = self._planLegList[i], self._planLegList[i+1]
            if thisLeg.getArrStation() != nextLeg.getDepStation():
                return False
            if not thisLeg.isMaint() and not nextLeg.isMaint():
                if thisLeg.getArrTime() + ut.util.turnTime > nextLeg.getDepTime():
                    return False
            else:
                if thisLeg.getArrTime() > nextLeg.getDepTime():
                    return False
        depCloseList, arrCloseList = [], []
        for thisLeg in self._planLegList:
            if thisLeg.isMaint():
                if thisLeg.getAircraft() != self:
                    return False
            else:
                depCloseList = thisLeg.getDepStation().getCloseTimeList()
                for _depClose in depCloseList:
                    if thisLeg.getDepTime() >= _depClose[0] and thisLeg.getDepTime() < _depClose[1]:
                        return False
                arrCloseList = thisLeg.getArrStation().getCloseTimeList()
                for _arrClose in arrCloseList:
                    if thisLeg.getArrTime() >= _arrClose[0] and thisLeg.getArrTime() < _arrClose[1]:
                        return False
        return True

class OperLeg:
    def __init__(self, leg: Leg, aircraft: Aircraft = None):
        self._leg = leg
        self._depTime = leg.getDepTime()
        self._arrTime = leg.getArrTime()
        self._operAircraft = aircraft

    def print(self) -> None:
        if self._operAircraft != None:
            self._operAircraft.print()
        self._leg.print()
        depTime = self._depTime - ut.TIMEDIFF
        arrTime = self._arrTime - ut.TIMEDIFF
        print("ODp %s" % time.ctime(depTime))
        print("OAr %s" % time.ctime(arrTime))
        print()

    def getLeg(self) -> Leg:
        return self._leg
    
    def getOpDepTime(self) -> float:
        return self._depTime

    def getOpArrTime(self) -> float:
        return self._arrTime
    
    def getPrintDepTime(self) -> float:
        return self._depTime
    
    def getPrintArrTime(self) -> float:
        return self._arrTime
    
    def setOpDepTime(self, t: float) -> None:
        self._depTime = t

    def setOpArrTime(self, t: float) -> None:
        self._arrTime = t

    def getScheDepTime(self) -> float:
        return self._leg.getDepTime()
    
    def getScheArrTime(self) -> float:
        return self._leg.getArrTime()
    
    def setOpAircraft(self, aircraft: Aircraft) -> None:
        self._operAircraft = aircraft
    
    def getOperAircraft(self) -> Aircraft:
        return self._operAircraft
    
    def getScheAircraft(self) -> Aircraft:
        return self._leg.getAircraft()

class Schedule:
    def __init__(self, stationList: List[Station], aircraftList: List[Aircraft], legList: List[Leg]):
        self._reversePost = Stack()
        self._topOrderList = []
        self._stationList, self._aircraftList, self._legList = stationList, aircraftList, legList
        self._maintList = [_leg for _leg in legList if _leg.isMaint()]
        self._flightList = [_leg for _leg in legList if not _leg.isMaint()]
        print("****** Total Number of Airports is " + str(len(stationList)) + " ******")
        for _station in self._stationList:
            _station.print()
        print("****** Total Number of Aircraft is " + str(len(aircraftList)) + " ******")
        for _aircraft in self._aircraftList:
            _aircraft.print()
        print("******* Total Number of Maints is " + str(len(self._maintList)) + " *******")
        for _maint in self._maintList:
            _maint.print()
        print("******* Total Number of Flights is " + str(len(self._flightList)) + " *******")
        for _flight in self._flightList:
            _flight.print()
        self.setAdjascentLeg()

    def setAdjascentLeg(self) -> None:
        for _leg1 in self._legList:
            for _leg2 in self._legList:
                if _leg1.isMaint() and _leg2.isMaint():
                    if _leg1.getArrStation() == _leg2.getDepStation() and _leg1.getArrTime() <= _leg2.getDepTime() \
                    and _leg1.getAircraft() == _leg2.getAircraft():
                        _leg1.pushNextLeg(_leg2)
                        _leg2.pushPrevLeg(_leg1)
                if _leg1.isMaint() and not _leg2.isMaint():
                    if _leg1.getArrStation() == _leg2.getDepStation() and _leg1.getDepTime() <= _leg2.getDepTime() \
                    and _leg1.getArrTime() - _leg2.getDepTime() <= ut.util.maxDelayTime:
                        _leg1.pushNextLeg(_leg2)
                        _leg2.pushPrevLeg(_leg1)
                if not _leg1.isMaint() and _leg2.isMaint():
                    if _leg1.getArrStation() == _leg2.getDepStation() and _leg1.getArrTime() <= _leg2.getDepTime():
                        _leg1.pushNextLeg(_leg2)
                        _leg2.pushPrevLeg(_leg1)
                if not _leg1.isMaint() and not _leg2.isMaint():
                    if _leg1.getArrStation() == _leg2.getDepStation() and _leg1.getArrTime() - _leg2.getDepTime() <= ut.util.maxDelayTime:
                        if _leg1.getDepTime() < _leg2.getDepTime():
                            _leg1.pushNextLeg(_leg2)
                            _leg2.pushPrevLeg(_leg1)
                        if _leg1.getDepTime() == _leg2.getDepTime():
                            if _leg1.getId() < _leg2.getId():
                                _leg1.pushNextLeg(_leg2)
                                _leg2.pushPrevLeg(_leg1)
        self._connectionSize = 0
        for _leg in self._legList:
            self._connectionSize += len(_leg.getNextLegList())
        print("############## TOTAL CONNECTION " + str(self._connectionSize) + " ###############")

    def computeTopOrder(self) -> None:
        for _leg in self._legList:
            if not _leg.isVisited():
                self.dfs(_leg)
        while self._reversePost.size() > 0:
            self._topOrderList.append(self._reversePost.pop())

    def getTopOrderList(self) -> List[Leg]:
        return self._topOrderList
    
    def getConnectionSize(self) -> int:
        return self._connectionSize
    
    def dfs(self, leg) -> None:
        leg.setIsVisited(True)
        nextLegList = leg.getNextLegList()
        for _leg in nextLegList:
            if not _leg.isVisited():
                self.dfs(_leg)
        self._reversePost.push(leg)

class Lof:
    _count = 0

    def __init__(self):
        self._aircraft = None
        self._cost = 0
        self._id = Lof._count
        Lof._count += 1
        self._legList = []
        self._maintList = []

    def pushLeg(self, leg: OperLeg) -> None:
        self._legList.append(leg)
        if leg.getLeg().isMaint():
            self._maintList.append(leg)

    def popLeg(self) -> None:
        self._legList.pop()
    
    def getSize(self) -> int:
        return len(self._legList)
    
    def setLegList(self, legList: List[OperLeg]) -> None:
        self._legList = legList

    def getLegList(self) -> List[OperLeg]:
        return self._legList
    
    def setAircraft(self, aircraft: Aircraft) -> None:
        self._aircraft = aircraft

    def getAircraft(self) -> Aircraft:
        return self._aircraft
    
    def computeLofCost(self) -> None:
        self._cost = 0
        for _leg in self._legList:
            if not _leg.getLeg().isMaint():
                self._cost += (_leg.getOpDepTime() - _leg.getScheDepTime())/60.0 * ut.util.w_fltDelay
            if _leg.getScheAircraft() != self._aircraft:
                self._cost += ut.util.w_fltSwap

    def containMaint(self) -> bool:
        return len(self._maintList) > 0

    def setCost(self, cost: float) -> None:
        self._cost = cost

    def getCost(self) -> float:
        return self._cost

    def getLastLeg(self) -> OperLeg:
        return self._legList[-1]
    
    def print(self) -> None:
        print("************ LOF ************")
        print("Lof ID is ", self._id)
        if self._aircraft != None:
            self._aircraft.print()
        else:
            print("Tail is NULL")
        print("Leg List Size is %d cost %d" % (len(self._legList), self._cost))
        print()
        for _leg in self._legList:
            _leg.print()
        print("*****************************")

    def getDepStation(self) -> Station:
        return self._legList[0].getLeg().getDepStation()
    
    def getArrStation(self) -> Station:
        return self._legList[-1].getLeg().getArrStation()
    
    def getOperDepTime(self) -> float:
        return self._legList[0].getOpDepTime()

    def getOperArrTime(self) -> float:
        if self._legList[-1].getLeg().isMaint():
            return self._legList[-1].getOrArrTime()
        return self._legList[-1].getOpArrTime()

    def getDepTime(self) -> float:
        return self._legList[0].getPrintDepTime()

    def computeReducedCost(self) -> None:
        self._reducedCost = 0
        sumLegDual = 0
        for _leg in self._legList:
            sumLegDual += _leg.getLeg().getDual()
        self._reducedCost = self._cost - sumLegDual - self._aircraft.getDual()

    def getReducedCost(self) -> float:
        return self._reducedCost

    def setId(self, id: int) -> None:
        self._id = id

    def getId(self) -> int:
        return self._id
    
    def compareDepTimeKey(self) -> float:
        return self.getOperDepTime()