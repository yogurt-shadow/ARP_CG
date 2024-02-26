# Copyright 2023, Gurobi Optimization, LLC

# This example reads an LP model from a file and solves it.
# If the model is infeasible or unbounded, the example turns off
# presolve and solves the model again. If the model is infeasible,
# the example computes an Irreducible Inconsistent Subsystem (IIS),
# and writes it to a file

# Input: 
# python3 input_path output_path cutoff

import sys
import gurobipy as gp
from gurobipy import GRB

# Read and solve model
if __name__ == "__main__":
    model1 = gp.read("pp_0.lp")
    model1.setParam(GRB.Param.Threads, 1)
    model1.setParam("OutputFlag", 0)
    model1.optimize()
    print(model1.ObjVal)