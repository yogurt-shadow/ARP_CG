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
    model1 = gp.read("../LP/PY/pp_%d.lp" % int(sys.argv[1]))
    model1.setParam(GRB.Param.Threads, 1)
    model1.setParam("OutputFlag", 0)
    model1.optimize()
    print(model1.ObjVal)
    model1.write("../LP/PY/pp_%d.mps" % int(sys.argv[1]))
    for cons in model1.getConstrs():
        print("%s: %d" % (cons.ConstrName, cons.Pi))

    model2 = gp.read("../LP/CPP/cc_%d.lp" % int(sys.argv[1]))
    model2.setParam(GRB.Param.Threads, 1)
    model2.setParam("OutputFlag", 0)
    model2.optimize()
    print(model2.ObjVal)
    model2.write("../LP/CPP/cc_%d.mps" % int(sys.argv[1]))
    for cons in model2.getConstrs():
        print("%s: %d" % (cons.ConstrName, cons.Pi))


# if model.Status == GRB.INF_OR_UNBD:
#     # Turn presolve off to determine whether model is infeasible
#     # or unbounded
#     model.setParam(GRB.Param.Presolve, 0)
#     model.optimize()

# if model.Status == GRB.OPTIMAL:
#     print('Optimal objective: %g' % model.ObjVal)
#     model.write('model.sol')
#     sys.exit(0)
# elif model.Status != GRB.INFEASIBLE:
#     with open() as f:
#         f.write('Optimization was stopped with status %d' % model.Status)

# model.write(sys.argv[2])
# with open(sys.argv[2], 'a') as f:
#     f.write("#Time: %f" % model.Runtime)

# # Model is infeasible - compute an Irreducible Inconsistent Subsystem (IIS)

# print('')
# print('Model is infeasible')
# model.computeIIS()
# model.write("model.ilp")
# print("IIS written to file 'model.ilp'")