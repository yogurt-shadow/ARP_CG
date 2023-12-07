import util as ut
from typing import List
from Leg import Leg
from Aircraft import Aircraft
from OperLeg import OperLeg
from Station import Station

class Lof:
    _count = 0

    def __init__(self):
        self._aircraft = None
        self._cost = 0
        self._id = _count
        _count += 1

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
            if not _leg.isMaint():
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
        print("Lof ID is ")
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