import gurobipy  as grb
GRB = grb.GRB
m = grb.Model()

x = m.addVar(0.0, 1.0, vtype=GRB.CONTINUOUS)
y = m.addVar(0.0, 1.0, vtype=GRB.CONTINUOUS)
# add constraints so that y >= |x - 0.75|
m.addConstr(y >= x-0.75)
m.addConstr(y >= 0.75 - x)
m.setObjective(y)
m.optimize()
print(x.x)
# 0.75
x.vtype=GRB.BINARY
m.optimize()
print(x.x)
# 1.0