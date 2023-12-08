from Structures import Station, Aircraft, Leg, OperLeg, SubNode, Lof
from typing import List
from Stack import Stack
import Util as ut
import sys
import gurobipy as gp
from gurobipy import GRB

class Model:
    _count = 0

    def __init__(self, stationList: List[Station], aircraftList: List[Aircraft], legList: List[Leg], topOrderList: List[Leg]):
        self._stationList, self._aircraftList = stationList, aircraftList
        self._legList, self._topOrderList = legList, topOrderList
        self._tolerance = 0
        self._lofVar, self._legVar = [], [] # var
        self._coverRng, self._selectRng = [], [] # cons
        self._finalLofList , self._cancelLegList = [], [] # final solution
        self._initColumns = []
        self._tolerance = 0
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
        if nextLeg.isMaint():
            print("Error, nextLeg must be flight to compute delay!")
            sys.exit(0)
        nextLegDepTime = nextLeg.getDepTime() + delay
        nextLegArrTime = nextLeg.getArrTime() + delay
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
        nextLegArrTime = nextLeg.getArrTime() + delay
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
    def edgeProcessFltFltSubNode(self, subNode: SubNode, nextLeg: Leg, aircraft: Aircraft) -> None:
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
        subNodeList = thisLeg.getSubNodeList()
        for _subNode in subNodeList:
           self.edgeProcessFltMaintSubNode(_subNode, nextLeg, aircraft)

    def edgeProcessFltMaintSubNode(self, subNode: SubNode, nextLeg: Leg, aircraft: Aircraft) -> None:
        delay, edgeCost = 0, 0
        if nextLeg.getAircraft() == aircraft:
            if subNode.getOperArrTime() <= nextLeg.getDepTime():
                if nextLeg.getArrTime() > aircraft.getEndTime():
                    return
                edgeCost = 0 - nextLeg.getDual()
                newSubNode = SubNode(nextLeg, subNode, subNode.getSubNodeCost() + edgeCost, delay)
                nextLeg.insertSubNode(newSubNode)

    def edgeProcessMaintFlt(self, thisLeg: Leg, nextLeg: Leg, aircraft: Aircraft) -> None:
        subNodeList = thisLeg.getSubNodeList()
        for _subNode in subNodeList:
            self.edgeProcessMaintFltSubNode(_subNode, nextLeg, aircraft)
    
    def edgeProcessMaintFltSubNode(self, subNode: SubNode, nextLeg: Leg, aircraft: Aircraft) -> None:
        delay, edgeCost = 0, 0
        thisLeg = subNode.getLeg()
        if thisLeg.getAircraft() == aircraft:
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
        else:
            if len(thisLeg.getSubNodeList) > 0:
                print("Error, maintenance and aircraft mismatch; subNodeList of maintenance must be empty!")
                sys.exit(0)


    def exgeProcessMaintMaint(self, thisLeg: Leg, nextLeg: Leg, aircraft: Aircraft) -> None:
        subNodeList = thisLeg.getSubNodeList()
        for _subNode in subNodeList:
            self.edgeProcessMaintMaintSubNode(_subNode, nextLeg, aircraft)

    def edgeProcessMaintMaintSubNode(self, subNode: SubNode, nextLeg: Leg, aircraft: Aircraft) -> None:
        delay, edgeCost = 0, 0
        thisLeg = subNode.getLeg()
        if thisLeg.getAircraft() != nextLeg.getAircraft():
            print("Error, aircraft of two connected maintenances do not match")
            sys.exit(0)
        if nextLeg.isMaint():
            if nextLeg.getAircraft() == aircraft:
                if subNode.getOperArrTime() > nextLeg.getDepTime():
                    print("Error, maintenance cannot be delayed!")
                    sys.exit(0)
                if nextLeg.getArrTime() > aircraft.getEndTime():
                    return
                edgeCost = 0 - nextLeg.getDual()
                newSubNode = SubNode(nextLeg, subNode, subNode.getSubNodeCost() + edgeCost, delay)
                nextLeg.insertSubNode(newSubNode)
            else:
                if len(thisLeg.getSubNodeList()) > 0:
                    print("Error, thisLeg maintenance and aircraft do not match!")
                    sys.exit(0)
        else:
            print("Error, nextLeg must be maintenance!")
            sys.exit(0)

    def solveColGen(self) -> List[Lof]:
        self._initColumns = self.findInitColumns()
        print("###### initial Lofs have been generated ######")
        self.populateByColumn(self._initColumns)
        print(" ********************* LP SOLUTION 0 *********************")
        self.solve()
        print(" ********************* END IP SOLUTION 0 *********************")
        print()
        count = 1
        betterColumns = self.findNewColumns()
        while len(betterColumns) > 0:
            print(" ********************* LP SOLUTION " + str(count) + " *********************")
            self.addColumns(betterColumns)
            self._initColumns.extend(betterColumns)
            self.solve()
            print(" ********************* END IP SOLUTION " + str(count) + " *********************")
            print()
            count += 1
            betterColumns = self.findNewColumns()
        lofListSoln = self.solveIP()
        return lofListSoln

    def populateByColumn(self, _initColumns: List[Lof]) -> None:
        # init constraint - var coefficient matrix
        """
        cover_leg_mat[cons][i]: coefficient of variable i in constraint cons
        cover_lof_mat[cons][j]: coefficient of variable j in constraint cons
        """
        cover_leg_mat = [[0] * len(self._legList) for i in range(len(self._legList))]
        cover_lof_mat = [[0] * len(self._initColumns) for i in range(len(self._legList))]
        """
        select_leg_mat[cons][i]: coefficient of variable i in constraint cons
        select_lof_mat[cons][j]: coefficient of variable j in constraint cons
        """
        select_leg_mat = [[0] * len(self._legList) for i in range(len(self._aircraftList))]
        select_lof_mat = [[0] * len(self._initColumns) for i in range(len(self._aircraftList))]

        # add decision variable _legVar (y)
        for i in range(len(self._legList)):
            _leg = self._legList[i]
            varName = "y_" + str(_leg.getId())
            cancelCost = ut.util.w_cancelFlt
            if _leg.isMaint():
                cancelCost = ut.util.w_cancelMtc
            v = self._model.addVar(lb = 0, ub = 1, obj = cancelCost, name = varName, vtype = GRB.CONTINUOUS)
            self._legVar.append(v)
            cover_leg_mat[_leg.getId()][i] = 1

        # add decision variable _lofVar (x)
        for i in range(len(self._initColumns)):
            _col = self._initColumns[i]
            varName = "x_" + str(_col.getId())
            v = self._model.addVar(lb = 0, ub = 1, obj = _col.getCost(), name = varName, vtype = GRB.CONTINUOUS)
            self._lofVar.append(v)
            operLegList = _col.getLegList()
            for j in range(len(operLegList)):
                _leg = operLegList[j].getLeg()
                cover_lof_mat[_leg.getId()][i] = 1
            select_lof_mat[_col.getAircraft().getId()][i] = 1

        # cover constraint
        for i in range(len(self._legList)):
            consName = "cover_lg_" + str(self._legList[i].getId())
            expr = gp.LinExpr()
            # add cover-leg terms
            for j in range(len(self._legVar)):
                if cover_leg_mat[i][j] > 0:
                    expr.addTerms(cover_leg_mat[i][j], self._legVar[j])
            # add cover-lof terms
            for j in range(len(self._lofVar)):
                if cover_lof_mat[i][j] > 0:
                    expr.addTerms(cover_lof_mat[i][j], self._lofVar[j])
            # add constraint
            rng = self._model.addConstr(expr == 1, name = consName)
            self._coverRng.append(rng)

        # select constraint
        for i in range(len(self._aircraftList)):
            consName = "select_ac_" + str(self._aircraftList[i].getId())
            expr = gp.LinExpr()
            # add select-leg terms
            for j in range(len(self._legVar)):
                if select_leg_mat[i][j] > 0:
                    expr.addTerms(select_leg_mat[i][j], self._legVar[j])
            # add select-lof terms
            for j in range(len(self._lofVar)):
                if select_lof_mat[i][j] > 0:
                    expr.addTerms(select_lof_mat[i][j], self._lofVar[j])
            # add constraint
            slt = self._model.addConstr(expr <= 1, name = consName)
            self._selectRng.append(slt)

    def solve(self) -> None:
        name = "recovery_" + str(Model._count) + ".lp"
        Model._count += 1
        """
        -1=automatic,
        0=primal simplex,
        1=dual simplex,
        2=barrier,
        3=concurrent,
        4=deterministic concurrent, and
        5=deterministic concurrent simplex (deprecated; see ConcurrentMethod).
        """
        self._model.setParam(GRB.param.Method, 2)
        # _solver.setParam(IloCplex::BarCrossAlg, IloCplex::NoAlg)
        self._model.optimize()
        print()
        print("Number of leg variables is: " + str(len(self._legVar)))
        print("Number of lof variables is: " + str(len(self._lofVar)))
        print("Number of selection constraint is: " + str(len(self._selectRng)))
        print("Number of cover constraint is: " + str(len(self._coverRng)))
        print("Solution status: " + str(self._model.Status))
        print("Optimal value: " + str(self._model.ObjVal))

        # get leg dual
        legDual = self._model.getAttr('Pi', self._coverRng)
        # set leg dual
        for i in range(len(self._legList)):
            if i != self._legList[i].getId():
                print("Error, leg index mismatch when get dual")
                sys.exit(0)
            self._legList[i].setDual(legDual[i])
        
        # get aircraft dual
        aircraftDual = self._model.getAttr('Pi', self._selectRng)
        # set aircraft dual
        for i in range(len(self._aircraftList)):
            if i != self._aircraftList[i].getId():
                print("Error, aircraft index mismatch when get dual")
                sys.exit(0)
            self._aircraftList[i].setDual(aircraftDual[i])



    def addColumns(self, _betterColumns: List[Lof]) -> None:
        for _col in _betterColumns:
            obj = _col.getCost()
            OperLegList = _col.getLegList()
            coeffs, constrs = [], []
            for _operLeg in OperLegList:
                _leg = _operLeg.getLeg()
                coeffs.append(1)
                constrs.append(self._coverRng[_leg.getId()])
            coeffs.append(1)
            constrs.append(self._selectRng[_col.getAircraft().getId()])
            varName = "x_" + str(_col.getId())
            v = self._model.addVar(lb = 0, ub = 1, obj = obj, name = varName, vtype = GRB.CONTINUOUS, column = gp.Column(coeffs, constrs))
            self._lofVar.append(v)

    def solveIP(self) -> List[Lof]:
        print(" ********************* FINAL IP SOLUTION *********************")
        for v in self._model.getVars():
            v.setAttr('VType', GRB.BINARY)
        self._model.write("recovery.lp")
        self._model.optimize()
        print()
        print("Number of leg variables is: " + str(len(self._legVar)))
        print("Number of lof variables is: " + str(len(self._lofVar)))
        print("Number of selection constraint is: " + str(len(self._selectRng)))
        print("Number of cover constraint is: " + str(len(self._coverRng)))
        print()
        print("Solution status: " + str(self._model.Status))
        print("Optimal value: " + str(self._model.ObjVal))
        lofVarSoln = [v.x for v in self._lofVar]
        lofListSoln = []
        if len(self._initColumns) > 0:
            for i in range(0, len(lofVarSoln)):
                _sln = lofVarSoln[i]
                if _sln <= 1.0001 and _sln >= 0.9999:
                    lofListSoln.append(self._initColumns[i])
                    self._initColumns[i].print()
        legVarSoln = [v.x for v in self._legVar]
        print()
        print(" ********************* END FINAL IP SOLUTION *********************")
        return lofListSoln
    