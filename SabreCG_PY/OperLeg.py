import util as ut
from typing import List
from Aircraft import Aircraft
from Leg import Leg

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
        print("ODp %s", depTime)
        print("OAr %s" % arrTime)

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