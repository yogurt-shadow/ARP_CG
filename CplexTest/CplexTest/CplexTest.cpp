//给大家一个测试代码

#include <ilcplex/ilocplex.h>



int main() {

    IloEnv env;

    try {

        IloModel model(env);

        IloCplex cplex(model);



        // 定义决策变量

        IloNumVar x(env, 0.0, IloInfinity, ILOFLOAT, "x");

        IloNumVar y(env, 0.0, IloInfinity, ILOFLOAT, "y");



        // 添加决策变量到模型中

        model.add(x);

        model.add(y);



        // 设置目标函数为最小化

        IloObjective obj = IloMinimize(env);

        obj.setExpr(3 * x + 2 * y);

        model.add(obj);



        // 添加约束条件：2 * x + y >= 10

        model.add(2 * x + y >= 10);



        // 求解模型

        cplex.solve();



        // 获取求解结果

        std::cout << "Objective value = " << cplex.getObjValue() << std::endl;

        std::cout << "x = " << cplex.getValue(x) << std::endl;

        std::cout << "y = " << cplex.getValue(y) << std::endl;

    }
    catch (IloException& e) {

        std::cerr << "Concert exception caught: " << e << std::endl;

    }
    catch (...) {

        std::cerr << "Unknown exception caught" << std::endl;

    }



    env.end();

    return 0;

}