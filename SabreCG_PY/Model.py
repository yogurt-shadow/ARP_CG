from Lof import Lof
from OperLeg import OperLeg
from Aircraft import Aircraft
from typing import List
from Leg import Leg
from Station import Station
from SubNode import SubNode
from Stack import Stack
import util as ut
import sys
import numpy as np
import gurobipy as gp
from gurobipy import GRB

class Model:
    _count = 0

    def __init__(self, stationList: List[Station], aircraftList: List[Aircraft], legList: List[Leg], topOrderList: List[Leg]):
        self._stationList, self._aircraftList = stationList, aircraftList
        self._legList, self._topOrderList = legList, topOrderList
        # initialize gurobi
        self._model = gp.Model()

    def findNewColumns(self) -> List[Lof]:
        betterLof, tempLof = [], []
        for _aircraft in self._aircraftList:
            tempLof = self.findNewMultiColumns(_aircraft)
            if len(tempLof) > 0:
                betterLof.extend(tempLof)
        print("Number of Better Lofs is " + str(len(betterLof)))
        print()
        return betterLof
    
    def findNewMultiColumns(self, aircraft: Aircraft) -> List[Lof]:
        betterLof, depLegList = [], aircraft.getDepStation().getDepLegList()
        for _depLeg in depLegList:
            self.edgeProcessFlt(_depLeg, aircraft)
        depMaintList = aircraft.getDepStation().getMainList()
        for _depMaint in depMaintList:
            self.edgeProcessMaint(_depMaint, aircraft)
        for thisLeg in self._topOrderList:
            for nextLeg in thisLeg.getNextLegList():
                if not thisLeg.isMaint() and not nextLeg.isMaint():
                    self.edgeProcessFltFlt(thisLeg, nextLeg, aircraft)
                if not thisLeg.isMaint() and nextLeg.isMaint():
                    self.edgeProcessFltMaint(thisLeg, nextLeg, aircraft)
                if thisLeg.isMaint() and not nextLeg.isMaint():
                    self.edgeProcessMaintFlt(thisLeg, nextLeg, aircraft)
                if thisLeg.isMaint() and nextLeg.isMaint():
                    self.exgeProcessMaintMaint(thisLeg, nextLeg, aircraft)
        tmpSubNodeList, arrLegList = [], aircraft.getArrStation().getArrLegList()
        for _arrLeg in arrLegList:
            for _subNode in _arrLeg.getSubNodeList():
                tmpSubNodeList.append(_subNode)
        arrMaintList = aircraft.getArrStation().getMainList()
        for _arrMaint in arrMaintList:
            for _subNode in _arrMaint.getSubNodeList():
                tmpSubNodeList.append(_subNode)
        if len(tmpSubNodeList) == 0:
            print("Warning, subproblem found no feasible LoF.")
            for _leg in self._legList:
                _leg.resetLeg()
            return betterLof
        tmpSubNodeList.sort(key = lambda x: x.CostKey())
        if tmpSubNodeList[0].getSubNodeCost() - aircraft.getDual() >= -0.0001:
            for _leg in self._legList:
                _leg.resetLeg()
            return betterLof
        tmp_count = 0
        for subNode in tmpSubNodeList:
            if tmp_count < ut.util.newamount:
                if subNode.getSubNodeCost() - aircraft.getDual() < -0.0001:
                    subNodeSelect = Stack()
                    tempSubNode = subNode
                    while tempSubNode != None:
                        subNodeSelect.push(tempSubNode)
                        tempSubNode = tempSubNode.getParentSubNode()
                    tempLeg, tempOperLeg = None, None
                    newLof = Lof()
                    newLof.setAircraft(aircraft)
                    while len(subNodeSelect) > 0:
                        tempSubNode = subNodeSelect.peek()
                        tempLeg = tempSubNode.getLeg()
                        tempOperLeg = OperLeg(tempLeg, aircraft)
                        tempOperLeg.setOpDepTime(tempSubNode.getOperDepTime())
                        tempOperLeg.setOpArrTime(tempSubNode.getOperArrTime())
                        newLof.pushLeg(tempOperLeg)
                        subNodeSelect.pop()
                    newLof.computeLofCost()
                    newLof.computeReducedCost()
                    error = newLof.getReducedCost() - (subNode.getSubNodeCost() - aircraft.getDual())
                    error = abs(error) / min(abs(newLof.getReducedCost()), abs(subNode.getSubNodeCost() - aircraft.getDual()))
                    if error > 0.0001:
                        print("newLof->getReducedCost() = " + str(newLof.getReducedCost()))
                        print("minCost - aircraft->getDual() = " + str(subNode.getSubNodeCost() - aircraft.getDual()))
                        print()
                        print("Error, subproblem reduced cost and minCost not match")
                        print("minCost is = " + str(subNode.getSubNodeCost()))
                        print("aircraft getDual = " + str(aircraft.getDual()))
                        newLof.print()
                        print("******* dual of legs are: *******")
                        lofOperLegList = newLof.getLegList()
                        for i in range(len(newLof.getSize())):
                            print("dual of leg %d  is %d" % (i, lofOperLegList[i].getLeg().getDual()))
                        break
                    betterLof.append(newLof)
                    tmp_count += 1
            else:
                break
        for _leg in self._legList:
            _leg.resetLeg()
        return betterLof
    
    def findNewOneColumn(self, aircraft: Aircraft) -> Lof:
        depLegList = aircraft.getDepStation().getDepLegList()
        for _depLeg in depLegList:
            self.edgeProcessFlt(_depLeg, aircraft)
        depMaintList = aircraft.getDepStation().getMainList()
        for _depMaint in depLegList:
            self.edgeProcessMaint(_depMaint, aircraft)
        for thisLeg in self._topOrderList:
            for nextLeg in thisLeg.getNextLegList():
                if not thisLeg.isMaint() and not nextLeg.isMaint():
                    self.edgeProcessFltFlt(thisLeg, nextLeg, aircraft)
                if not thisLeg.isMaint() and nextLeg.isMaint():
                    self.edgeProcessFltMaint(thisLeg, nextLeg, aircraft)
                if thisLeg.isMaint() and not nextLeg.isMaint():
                    self.edgeProcessMaintFlt(thisLeg, nextLeg, aircraft)
                if thisLeg.isMaint() and nextLeg.isMaint():
                    self.edgeProcessMaintMaint(thisLeg, nextLeg, aircraft)
        minCostSubNode = None
        minCost = float('inf')
        arrLegList = aircraft.getArrStation().getArrLegList()
        for _arrLeg in arrLegList:
            for _subNode in _arrLeg.getSubNodeList():
                if _subNode.getSubNodeCost() < minCost:
                    minCost = _subNode.getSubNodeCost()
                    minCostSubNode = _subNode
        arrMaintList = aircraft.getArrStation().getMainList()
        for _arrMaint in arrMaintList:
            for _subNode in _arrMaint.getSubNodeList():
                if _subNode.getSubNodeCost() < minCost:
                    minCost = _subNode.getSubNodeCost()
                    minCostSubNode = _subNode
        if minCostSubNode == None:
            print("Warning, subproblem found no feasible LoF.")
            for _leg in self._legList:
                _leg.resetLeg()
            return None
        print("reduced cost by subproblem aircraft %d is %d" % (aircraft.getId(), minCost - aircraft.getDual()))
        if minCost - aircraft.getDual() >= -0.0001:
            for _leg in self._legList:
                _leg.resetLeg()
            return None
        subNodeSelect = Stack()
        tempSubNode = minCostSubNode
        while tempSubNode != None:
            subNodeSelect.push(tempSubNode)
            tempSubNode = tempSubNode.getParentSubNode()
        tempLeg, tempOperLeg = None, None
        newLof = Lof()
        newLof.setAircraft(aircraft)
        while len(subNodeSelect) > 0:
            tempSubNode = subNodeSelect.peek()
            tempLeg = tempSubNode.getLeg()
            tempOperLeg = OperLeg(tempLeg, aircraft)
            tempOperLeg.setOpDepTime(tempSubNode.getOperDepTime())
            tempOperLeg.setOpArrTime(tempSubNode.getOperArrTime())
            newLof.pushLeg(tempOperLeg)
            subNodeSelect.pop()
        newLof.computeLofCost()
        newLof.computeReducedCost()

        error = newLof.getReducedCost() - (minCost - aircraft.getDual())
        error = abs(error) / min(abs(newLof.getReducedCost()), abs(minCost - aircraft.getDual()))
        if error > 0.0001:
            print("newLof->getReducedCost() = " + str(newLof.getReducedCost()))
            print("minCost - aircraft->getDual() = " + str(minCost - aircraft.getDual()))
            print("Error, subproblem reduced cost and minCost not match")
            print("minCost is = " + str(minCost))
            print("aircraft getDual = " + str(aircraft.getDual()))
            newLof.print()
            print("******* dual of legs are: *******")
            lofOperLegList = newLof.getLegList()
            for i in range(len(newLof.getSize())):
                print("dual of leg %d  is %d" % (i, lofOperLegList[i].getLeg().getDual()))
            sys.exit(0)
        for _leg in self._legList:
            _leg.resetLeg()
        return newLof

    def computeFlightDelay(self, subNode: SubNode, nextLeg: Leg) -> float:
        delay = 0
        thisLeg = subNode.getLeg()
        if nextLeg.isMaint():
            print("Error, nextLeg must be flight to compute delay!")
            sys.exit(0)
        if thisLeg.isMaint():
            if subNode.getOperArrTime() > nextLeg.getDepTime():
                delay = subNode.getOperArrTime() - nextLeg.getDepTime()
        else:
            if subNode.getOperArrTime() + ut.util.turnTime > nextLeg.getDepTime():
                delay = subNode.getOperArrTime() + ut.util.turnTime - nextLeg.getDepTime()
        delay2 = self.delayByAirportClose(nextLeg, delay)
        return delay + delay2

    def delayByAirportClose(self, nextLeg: Leg, delay: float) -> None:
        pass

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
    
    def edgeProcessMaint(self, nextLeg: Leg, aircraft: Aircraft) -> None:
        pass

    def edgeProcessFlt(self, nextLeg: Leg, aircraft: Aircraft) -> None:
        if nextLeg.isMaint():
            print("Error, input of edgeProcessFlt must be flight.")
            return
        delay, edgeCost = 0, 0
        if aircraft.getStartTime() > nextLeg.getDepTime():
            delay = aircraft.getStartTime() - nextLeg.getDepTime()
        delay2 = self.delayByAirportClose(nextLeg, delay)
        return delay + delay2
    
    def delayByAirportClose(self, nextLeg: Leg, delay: float) -> float:
        if nextLeg.isMaint():
            print("Error, nextLeg must be flight to compute delay!")
            sys.exit(0)
        nextLegDepTime = nextLeg.getDepTime() + delay
        nextLegArrTime = nextLeg.getArrTime11 + delay
        delay2 = 0
        depCloseList = nextLeg.getDepStation().getCloseTimeList()
        arrCloseList = nextLeg.getArrStation().getCloseTimeList()
        stopFlag1, stopFlag2 = False, False
        while not stopFlag1 or not stopFlag2:
            stopFlag1 = True
            for _depClose in depCloseList:
                if nextLegDepTime + delay2 >= _depClose[0] and nextLegDepTime + delay2 < _depClose[1]:
                    delay2 = _depClose[1] - nextLegDepTime
                    stopFlag1 = False
                    break
            stopFlag2 = True
            for _arrClose in arrCloseList:
                if nextLegArrTime + delay2 >= _arrClose[0] and nextLegArrTime + delay2 < _arrClose[1]:
                    delay2 = _arrClose[1] - nextLegArrTime
                    stopFlag2 = False
                    break
        return delay2

    def edgeProcessFltFlt(self, thisLeg: Leg, nextLeg: Leg, aircraft: Aircraft) -> None:
        subNodeList = thisLeg.getSubNodeList()
        for _subNode in subNodeList:
            self.edgeProcessFltFltSubNode(_subNode, nextLeg, aircraft)

    # helper function
    def edgeProcessFltFlt(self, subNode: SubNode, nextLeg: Leg, aircraft: Aircraft) -> None:
        delay, edgeCost = 0, 0
        delay = self.computeFlightDelay(subNode, nextLeg)
        edgeCost = delay / 60.0 * ut.util.w_fltDelay - nextLeg.getDual()
        if delay > ut.util.maxDelayTime:
            return
        if nextLeg.getArrTime() + delay > aircraft.getEndTime():
            return
        if nextLeg.getAircraft() != aircraft:
            edgeCost += ut.util.w_fltSwap
        newSubNode = SubNode(nextLeg, subNode, subNode.getSubNodeCost() + edgeCost, delay)
        nextLeg.insertSubNode(newSubNode)

    def edgeProcessFltMaint(self, thisLeg: Leg, nextLeg: Leg, aircraft: Aircraft) -> None:
        pass

    def edgeProcessMaintFlt(self, thisLeg: Leg, nextLeg: Leg, aircraft: Aircraft) -> None:
        pass

    def exgeProcessMaintMaint(self, thisLeg: Leg, nextLeg: Leg, aircraft: Aircraft) -> None:
        pass

    def solveColGen(self) -> List[Lof]:
        pass

    def populateByColumn(self, _initColumns: List[Lof]) -> None:
        pass

    def solve(self) -> None:
        pass

    def addColumns(self, _betterColumns: List[Lof]) -> None:
        pass

    def solveIP(self) -> List[Lof]:
        pass
    