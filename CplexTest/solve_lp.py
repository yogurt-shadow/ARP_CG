# This example formulates and solves the following simple MIP model:
#  maximize
#        x +   y + 2 z
#  subject to
#        x + 2 y + 3 z <= 4
#        x +   y       >= 1
#        x, y, z binary

from gurobipy import *
import os

try:

    # Create a new model
    m1 = Model("mip1")
    if os.path.exists("../LP/PY/pp_0.lp"):
        print("File exists")
    m1.read("../LP/PY/pp_0.lp")
    m1.optimize()

    for v in m1.getVars():
        print('%s %g' % (v.varName, v.x))

    print('Obj: %g' % m1.objVal)

    # Create a new model
    m2 = Model("mip2")
    m2.read("../LP/CPP/cc_0.lp")
    m2.optimize()

    for v in m2.getVars():
        print('%s %g' % (v.varName, v.x))

    print('Obj: %g' % m2.objVal)

except GurobiError as e:
    print('Error code ' + str(e.errno) + ": " + str(e))

except AttributeError:
    print('Encountered an attribute error')
