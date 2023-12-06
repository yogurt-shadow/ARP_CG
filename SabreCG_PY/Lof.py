import util as ut

class Lof:
    _count = 0

    def __init__(self):
        self._aircraft = None
        self._cost = 0
        self._id = _count
        _count += 1

    def pushLeg(leg):
        pass

    def popLeg(self):
        self._legList.pop()
    
    def getSize(self):
        return len(self._legList)
    
    def setLegList(self, legList):
        self._legList = legList

    def getLegList(self):
        return self._legList
    
    def setAircraft(self, aircraft):
        self._aircraft = aircraft

    def getAircraft(self):
        return self._aircraft
    
    def computeLofCost(self):
        self._cost = 0
        for _leg in self._legList:
            if not _leg.isMaint():
                self._cost += (_leg.getOpDepTime() - _leg.getScheDepTime())/60.0 * ut.util.w_fltDelay
            if _leg.getScheAircraft() != self._aircraft:
                self._cost += ut.util.w_fltSwap

    def containMaint(self):
        return len(self._maintList) > 0

    def setCost(self, cost):
        self._cost = cost

    def getCost(self):
        return self._cost

    def getLastLeg(self):
        return self._legList[-1]
    
    def print(self):
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

    def getDepStation(self):
        return self._legList[0].getLeg().getDepStation()
    
    def getArrStation(self):
        return self._legList[-1].getLeg().getArrStation()
    
    def getOperDepTime(self):
        return self._legList[0].getOpDepTime()

    def getOperArrTime(self):
        if self._legList[-1].getLeg().isMaint():
            return self._legList[-1].getOrArrTime()
        return self._legList[-1].getOpArrTime()

    def getDepTime(self):
        return self._legList[0].getPrintDepTime()

    def computeReducedCost(self):
        self._reducedCost = 0
        sumLegDual = 0
        for _leg in self._legList:
            sumLegDual += _leg.getLeg().getDual()
        self._reducedCost = self._cost - sumLegDual - self._aircraft.getDual()

    def getReducedCost(self):
        return self._reducedCost

    def setId(self, id):
        self._id = id

    def getId(self):
        return self._id
    
    def compareDepTimeKey(self):
        return self.getOperDepTime()