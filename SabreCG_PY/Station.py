import util as ut
from Aircraft import Aircraft
from Leg import Leg
from typing import List

class Station:
    count = 0
    def __init__(self, name: str):
        self._name = name
        self._id = count
        count += 1

    def getName(self) -> str:
        return self._name
    
    def pushDepLeg(self, leg: Leg) -> None:
        self._depLegList.append(leg)

    def pushArrLeg(self, leg: Leg) -> None:
        self._arrLegList.append(leg)

    def pushDepAircraft(self, _aircraft: Aircraft) -> None:
        self._depAircraft.append(_aircraft)

    def getDepAircraftNumber(self) -> int:
        return len(self._depAircraft)
    
    def getDepLegList(self) -> List[Leg]:
        return self._depLegList
    
    def getArrLegList(self) -> List[Leg]:
        return self._arrLegList
    
    def pushMaint(self, maint: Leg) -> None:
        self._maintList.append(maint)

    def getMainList(self) -> List[Leg]:
        return self._maintList
    
    def pushCloseTime(self, closeTime: (float, float)) -> None:
        self._closeTimeList.append(closeTime)

    def getCloseTimeList(self) -> (float, float):
        return self._closeTimeList
    
    def print(self) -> None:
        print("Station %s Id %d" % (self._name, self._id))
        for _time in self._closeTimeList:
            begin, end = _time[0] - ut.TIMEDIFF, _time[1] - ut.TIMEDIFF
            print("Bgn Close " + begin)
            print("End Close " + end)

    def getId(self) -> int:
        return self._id
    
    def setLegNum(self, _size: int) -> None:
        self._legNum = _size