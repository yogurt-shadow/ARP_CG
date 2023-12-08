//给大家一个测试代码

#include <ilcplex/ilocplex.h>
#include <iostream>
#include <string>

using namespace std;


int main() {

    IloEnv _env;

    try {
        IloModel _model;
        IloCplex _solver;
        IloObjective _obj;
        IloNumVarArray _Var;
        IloRangeArray _Rng;

        _model = IloModel(_env, "Minimize");
        _obj = IloMinimize(_env);
        _Var = IloNumVarArray(_env);
        _Rng = IloRangeArray(_env);
        _solver = IloCplex(_model);

        _obj = IloAdd(_model, IloMinimize(_env));
        for (int i = 0; i < 5; i++) {
            string cons_name = "cover_lg_" + to_string(i);
            cout << "here " << i << endl;
            _Rng.add(IloRange(_env, 1, 1, cons_name.c_str()));
        }
        _model.add(_Rng);

        int rr[] = {1, 4, 0, 3, 2};

        for (int i = 0; i < 5; i++) {
            string varName = "y_" + to_string(i);
            _Var.add(IloNumVar(_obj(100) + _Rng[rr[i]](1), 0, 1, ILOFLOAT, varName.c_str()));
        }
        _solver.solve();
        _solver.exportModel("recovery.lp");
    }
    catch (IloException& e) {

        std::cerr << "Concert exception caught: " << e << std::endl;

    }
    catch (...) {

        std::cerr << "Unknown exception caught" << std::endl;

    }



    _env.end();

    return 0;

}