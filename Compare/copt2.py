from coptpy import * 

# Create COPT environment 
env = Envr()

# create model
model = env.createModel("LP example") 


model.read("pp_0.lp")

# solve and output the optimal solution
model.solve()

print("Objective value: {}".format(model.objval))
print("Variable solution:")

for var in model.getVars():
    print(" x[{0}]: {1}".format(var.index, var.x))
