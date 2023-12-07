from Stack import Stack
from typing import List
from Station import Station
from Leg import Leg
from Aircraft import Aircraft
import util as ut

class Schedule:
    def __init__(self, stationList: List[Station], aircraftList: List[Aircraft], legList: List[Leg]):
        self._reversePost = Stack()
        self._topOrderList = []
        self._stationList, self._aircraftList, self._legList = stationList, aircraftList, legList
        self._maintList = [_leg for _leg in legList if _leg.isMaint()]
        self._flightList = [_leg for _leg in legList if not _leg.isMaint()]
        print("****** Total Number of Airports is " + str(len(stationList) + " ******"))
        for _station in self._stationList:
            _station.print()
        print("****** Total Number of Aircraft is " + str(len(aircraftList) + " ******"))
        for _aircraft in self._aircraftList:
            _aircraft.print()
        print("****** Total Number of Maints is " + str(len(self._maintList) + " ******"))
        for _maint in self._maintList:
            _maint.print()
        print("****** Total Number of Flights is " + str(len(self._flightList) + " ******"))
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
        while len(self._reversePost) > 0:
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
        self._reversePost.append(leg)
    
