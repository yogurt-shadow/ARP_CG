from Structures import Station, Aircraft, Leg, OperLeg, SubNode, Lof
from typing import List
from Stack import Stack
import Util as ut
import sys
import gurobipy as gp
from gurobipy import GRB
import time
import os

class Model:
    _count = 0

    def __init__(self, stationList: list[Station], aircraftList: list[Aircraft], legList: list[Leg], topOrderList: list[Leg]):
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
        self.print()

    def print(self):
        print("print model")
        for ele in self._stationList:
            ele.print()
        for ele in self._aircraftList:
            ele.print()
        for ele in self._legList:
            ele.print()
        for ele in self._topOrderList:
            ele.print()

    