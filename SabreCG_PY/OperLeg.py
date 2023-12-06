import util as ut

class OperLeg:
    def __init__(self, leg, aircraft = None):
        self._leg = leg
        self._depTime = leg.getDepTime()
        self._arrTime = leg.getArrTime()
        self._operAircraft = aircraft

    def print(self):
        if self._operAircraft != None:
            self._operAircraft.print()
        self._leg.print()
        depTime = self._depTime - ut.TIMEDIFF
        arrTime = self._arrTime - ut.TIMEDIFF
        print("ODp %s", depTime)
        print("OAr %s" % arrTime)

    def getLeg(self):
        return self._leg
    
    def getOpDepTime(self):
        return self._depTime

    def getOpArrTime(self):
        return self._arrTime
    
    def getPrintDepTime(self):
        return self._depTime
    
    def getPrintArrTime(self):
        return self._arrTime
    
    def setOpDepTime(self, t):
        self._depTime = t

    def setOpArrTime(self, t):
        self._arrTime = t

    def getScheDepTime(self):
        return self._leg.getDepTime()
    
    def getScheArrTime(self):
        return self._leg.getArrTime()
    
    def setOpAircraft(self, aircraft):
        self._operAircraft = aircraft
    
    def getOperAircraft(self):
        return self._operAircraft
    
    def getScheAircraft(self):
        return self._leg.getAircraft()