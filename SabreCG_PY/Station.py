import util as ut

class Station:
    count = 0
    def __init__(self, name):
        self._name = name
        self._id = count
        count += 1

    def getName(self):
        return self._name
    
    def pushDepLeg(self, leg):
        self._depLegList.append(leg)

    def pushArrLeg(self, leg):
        self._arrLegList.append(leg)

    def pushDepAircraft(self, _aircraft):
        self._depAircraft.append(_aircraft)

    def getDepAircraftNumber(self):
        return len(self._depAircraft)
    
    def getDepLegList(self):
        return self._depLegList
    
    def getArrLegList(self):
        return self._arrLegList
    
    def pushMaint(self, maint):
        self._maintList.append(maint)

    def getMainList(self):
        return self._maintList
    
    def pushCloseTime(self, closeTime):
        self._closeTimeList.append(closeTime)

    def getCloseTimeList(self):
        return self._closeTimeList
    
    def print(self):
        print("Station %s Id %d" % (self._name, self._id))
        for _time in self._closeTimeList:
            begin, end = _time[0] - ut.TIMEDIFF, _time[1] - ut.TIMEDIFF
            print("Bgn Close " + begin)
            print("End Close " + end)

    def getId(self):
        return self._id
    
    def setLegNum(self, _size):
        self._legNum = _size