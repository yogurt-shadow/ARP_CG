from Lof import Lof
from OperLeg import OperLeg
import sys

class Model:
    _count = 0

    def __init__(self, stationList, aircraftList, legList, topOrderList):
        self._stationList, self._aircraftList = stationList, aircraftList
        self._legList, self._topOrderList = legList, topOrderList
        # initialize gurobi
        # TODO

    def findInitOneColumn(self, aircraft):
        aircraft.sortScheLegByDepTime()
        if aircraft.isPlanLegFeasible:
            newLof = Lof()
            newLof.setAircraft(aircraft)
            tempOperLeg = None
            planLegList = aircraft.getPlanLegList()
            for _plan in planLegList:
                tempOperLeg = OperLeg(_plan, aircraft)
                newLof.pushLeg(tempOperLeg)
            newLof.computeLofCost()
            if newLof.getCost() >= 0.0001 or newLof.getCost() <= -0.0001:
                print("Error, cost of initial lof must be zero")
                sys.exit(0)
            return newLof
        else:
            return None

    def findInitColumns(self):
        initColumns = []
        for _aircraft in self._aircraftList:
            tempLof = self.findInitOneColumn(_aircraft)
            if tempLof != None:
                initColumns.append(tempLof)
        print("Number of Initial Lofs is " + str(len(initColumns)))
        return initColumns
    