from pulp import LpProblem, lpSum, LpVariable, LpMinimize

# Create an LP problem
var, model = LpProblem.fromMPS("../LP/PY/pp_0.mps", LpMinimize)

# Read the LP model from an MPS file
# lp.readLP("../LP/PY/pp_0.lp")

# Solve the LP problem
model.solve()

# Display results
print("Status:", model.status)
print("Optimal Objective Value:", model.objective.value())
print("Optimal Values for Decision Variables:")
for var in model.variables():
    print(f"{var.name}: {var.value()}")

for name, c in list(model.constraints.items()):
    print(name, ":", c, "\t", c.pi, "\t\t", c.slack)


# Create an LP problem
var2, model2 = LpProblem.fromMPS("../LP/CPP/cc_0.mps", LpMinimize)

# Read the LP model2 from an MPS file
# lp.readLP("../LP/PY/pp_0.lp")

# Solve the LP problem
model2.solve()

# Display results
print("Status:", model2.status)
print("Optimal Objective Value:", model2.objective.value())
print("Optimal Values for Decision var2iables:")
for var2 in model2.variables():
    print(f"{var2.name}: {var2.value()}")

for name, c in list(model2.constraints.items()):
    print(name, ":", c, "\t", c.pi, "\t\t", c.slack)
