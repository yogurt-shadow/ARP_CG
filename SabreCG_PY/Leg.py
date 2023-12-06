import util as ut

class Leg:
    _count = 0

    def __init__(self, flightNum, depStation, arrStation, depTime, arrTime, aircraft):
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
    def initFlighytNum(self, flightNum):
        self._flightNum = flightNum
        self._depStation, self._arrStation = None, None
        self._depTime, self._arrTime = 0, 0
        self._aircraft = None
        self._id = -1
        return self
    
    def print(self):
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

    def resetLeg(self):
        flag = False
        size = len(self._subNodeList)
        for i in range(size):
            self.popSubNode()
        flag = True
        return flag
    
    def insertSubNode(self, subNode):
        if len(self._subNodeList) == 0:
            self._subNodeList.append(subNode)
            return True
        pass
        # for node in self._subNodeList:
    