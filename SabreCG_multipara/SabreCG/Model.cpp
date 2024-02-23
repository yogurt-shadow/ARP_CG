#include "Model.h"

Model::Model(vector<Station *> stationList, vector<Aircraft *> aircraftList, vector<Leg *> legList, vector<Leg *> topOrderList):
	_stationList(stationList)
{
	_aircraftList = aircraftList;
	_legList = legList;
	_topOrderList = topOrderList;

	//* initialize CPLEX objects
	
	_model = IloModel(_env, "Minimize");
	_obj = IloMinimize(_env);
	_lofVar = IloNumVarArray(_env);
	_legVar = IloNumVarArray(_env);
	_coverRng = IloRangeArray(_env);
	_selectRng = IloRangeArray(_env);
	_solver = IloCplex(_model);
	
	_count = 0;

	_lpTime = 0;
	_ipTime = 0;
	_spTime = 0;
}

Model::~Model()
{
	_env.end();		// ɾ��Ilog����ض���
	//cout << "~Model::_env.end() executed" << endl;//����~Model
	
	try
	{
		for (int i = 0; i < _initColumns.size(); i++)		// ɾ���������ɹ���Lof
		{
			if(_initColumns[i] != NULL)
				delete _initColumns[i];
		}
		//cout << "~Model::initColunns deleted" << endl;//����~Model
	}
	catch(...)
	{
		cout << "Warning! Delete lof throws an exception!" << endl;
	}

	_initColumns.clear();
	//_betterColumns.clear();		// ���static��Ա
	clearBetterColumnsAll();
	_topOrderList.clear();		// ���static��Ա
}

//��ʼ����̬����
//int Model::_count = 0;
vector<Aircraft *> Model::_aircraftList;
vector<Leg *> Model::_legList;
vector<Lof* > Model::_betterColumns[THREADSIZE];
vector<Leg *> Model::_topOrderList;

vector<Lof* > Model::findInitColumns()
{
	vector<Lof* > initColumns;
	Lof* tempLof;

	for (int i = 0; i < _aircraftList.size(); i++)
	{
		tempLof = findInitOneColumn(_aircraftList[i]);
		if (tempLof != NULL)
		{
			initColumns.push_back(tempLof);
		}
	}

	cout << "Number of Initial Lofs is " << initColumns.size() << endl << endl;

	return initColumns;

	/* ��ʵû��initial LofҲ������

	/*
	Lof* tempLof = new Lof();
	
	// ���ֹ�����һ�³�ʼ��LoF, ����debug
	// һ�ܷɻ�
	// ��test case (12)derived from(3), or (13) or (14) Ϊ������ʼ��Lof�� lg0 -> lg2 -> lg3 (��Ȼlg0 -> lg1 -> lg4 -> lg3 Ӧ�ø���)
	tempLof->setAircraft( _aircraftList[0]);
	OperLeg* tempOperLeg = new OperLeg(_legList[0], _aircraftList[0]);
	tempLof->pushLeg(tempOperLeg);

	tempOperLeg = new OperLeg(_legList[2], _aircraftList[0]);
	tempOperLeg->setOpDepTime(25);
	tempOperLeg->setOpArrTime(35);
	tempLof->pushLeg(tempOperLeg);

	tempOperLeg = new OperLeg(_legList[3], _aircraftList[0]);
	tempOperLeg->setOpDepTime(45);
	tempOperLeg->setOpArrTime(55);
	tempLof->pushLeg(tempOperLeg);

	tempLof->computeLofCost();

	//tempLof->print();

	initColumns.push_back(tempLof);
	return initColumns;
	*/
}

Lof* Model::findInitOneColumn(Aircraft* aircraft)
{
	aircraft->sortScheLegByDepTime();
	if (aircraft->isPlanLegFeasible())
	{
		Lof* newLof = new Lof();
		newLof->setAircraft(aircraft);
		OperLeg* tempOperLeg = NULL;

		vector<Leg* > planLegList = aircraft->getPlanLegList();

		for (int i = 0; i < planLegList.size(); i++)
		{
			tempOperLeg = new OperLeg(planLegList[i], aircraft);
			newLof->pushLeg(tempOperLeg);
		}
		
		newLof->computeLofCost();
		if (newLof->getCost() >= 0.0001 || newLof->getCost() <= -0.0001)
		{
			cout << "Error, cost of initial lof must be zero" << endl;
			exit(0);
		}

		return newLof;
	}
	else
	{
		return NULL;
	}
}

void Model::addColumns(vector<Lof* > _betterColumns)		//*
{
	char str[16];

	for (int j = 0; j < _betterColumns.size(); j++)
	{
		IloNumColumn col(_env);
		col = _obj(_betterColumns[j]->getCost());

		vector<OperLeg* > operLegList = _betterColumns[j]->getLegList();
		for (int k = 0; k < operLegList.size(); k++)
		{
			col += _coverRng[operLegList[k]->getLeg()->getId()](1);
		}

		col += _selectRng[_betterColumns[j]->getAircraft()->getId()](1);

		itoa( _betterColumns[j]->getId(), str, 10);
		string varName = string("x_") + string(str);
		_lofVar.add(IloNumVar(col, 0, 1, ILOFLOAT, varName.c_str()));
		col.end();
	}
}

void Model::findNewColumns()
{
	//int threadIndex = 0;

	pair<long, long> indexList[THREADSIZE];
	HANDLE ghEvent[THREADSIZE];
	//int unit_size = _aircraftList.size() / THREADSIZE + 1;
	int unit_size = _aircraftList.size() / Util::threadSize + 1;

	// ��ʼ��ghEvent, ��ΪNULL
	for (int i = 0; i < THREADSIZE; i++)
	{
		ghEvent[i] = NULL;
	}

	//for (int j = 0; j < THREADSIZE; ++j)
	for (int j = 0; j < Util::threadSize; ++j)
	{
		indexList[j].first = unit_size*j;//each parallel thread deals with size/thread_size+1
		indexList[j].second = min(_aircraftList.size(), unit_size * (j + 1));
		ghEvent[j] = (HANDLE)_beginthreadex(NULL, 0, Model::findNewColumnsParallel, (void *)&indexList[j], 0, NULL);
	}
	//WaitForMultipleObjects(THREADSIZE, ghEvent, TRUE, INFINITE);
	WaitForMultipleObjects(Util::threadSize, ghEvent, TRUE, INFINITE);

	int betterCounter = 0;
	for (int i = 0; i < Util::threadSize; i++)
	{
		betterCounter += _betterColumns[i].size();
	}

	//cout << "Number of Better Lofs is " << _betterColumns.size() << endl << endl;
	cout << "Number of Better Lofs is " << betterCounter << endl << endl;

}

void Model::populateByColumn(vector<Lof* > _initColumns)
{
	char str[16];
	
	_obj = IloAdd(_model, IloMinimize(_env));

	//* cover constraint
	for (int i = 0; i < _legList.size(); i++)
	{
		itoa(_legList[i]->getId(), str, 10);
		string consName = string("cover_lg_") + string(str);
		//string consName = string("cover_") + _legList[i]->getFlightNum();

		_coverRng.add(IloRange(_env, 1, 1, consName.c_str()));
	}
	_model.add(_coverRng);

	//cout << "empty cover constraint complete" << endl;

	//* lof select constraint
	for (int i = 0; i < _aircraftList.size(); i++)
	{
		itoa(_aircraftList[i]->getId(), str, 10);
		string consName = string("select_ac_") + string(str);
		//string consName = string("select_") + _aircraftList[i]->getTail();

		_selectRng.add(IloRange(_env, -IloInfinity, 1, consName.c_str()));
	}
	_model.add(_selectRng);

	//* add decision variable _legVar
	//* ��ģ�����y
	for (int j = 0; j < _legList.size(); j++)
	{
		itoa(_legList[j]->getId(), str, 10);
		string varName = string("y_") + string(str);

		double cancelCost = Util::w_cancelFlt; // cancel flight��maint��cost��һ��
		if (_legList[j]->isMaint())
		{
			cancelCost = Util::w_cancelMtc;
		}

		_legVar.add(IloNumVar(_obj(cancelCost) + _coverRng[_legList[j]->getId()](1), 0, 1, ILOFLOAT, varName.c_str()));
	}

	//* add decision variable _lofVar
	//* ��ģ�����x
	for (int j = 0; j < _initColumns.size(); j++)
	{
		IloNumColumn col(_env);
		col = _obj(_initColumns[j]->getCost());

		vector<OperLeg* > operLegList = _initColumns[j]->getLegList();
		for (int k = 0; k < operLegList.size(); k++)
		{
			col += _coverRng[operLegList[k]->getLeg()->getId()](1);
		}

		col += _selectRng[_initColumns[j]->getAircraft()->getId()](1);

		itoa( _initColumns[j]->getId(), str, 10);
		string varName = string("x_") + string(str);
		_lofVar.add(IloNumVar(col, 0, 1, ILOFLOAT, varName.c_str()));
		col.end();
	}

	//cout << "end of model construction by initial columns" << endl;
}

vector<Lof* > Model::solveColGen()
{
	vector<Lof* > lofListSoln;

	_initColumns = findInitColumns();//##ԭ����feasible��column

	cout<<"###### initial Lofs have been generated ######"<<endl;

	populateByColumn(_initColumns);

	cout<<" ********************* LP SOLUTION 0 *********************"<<endl;
	clock_t st = clock();
	solve();
	clock_t et = clock();
	_lpTime = et - st;
	cout << "# Time for LP_0: " << (et - st) / CLK_TCK << " seconds" << endl;
	cout<<" ********************* END LP SOLUTION 0 *********************"<<endl<<endl;

	int count = 1;

	//_betterColumns.clear();
	clearBetterColumnsAll();
	st = clock();
	findNewColumns();
	et = clock();
	_spTime = et - st;
	cout << "# Time for SP_0: " << (et - st) / CLK_TCK << " seconds" << endl;

	//while (!_betterColumns.empty())
	while (hasBetterColumn())
	{
		cout<<" ********************* LP SOLUTION "<< count << " *********************"<<endl<<endl;

		/*
		//���Զ�Lof����
		cout << "before sort _betterColumns: " << endl;
		for(int i = 0; i < _betterColumns.size(); i++)
		{
			cout << "aircraft id " << _betterColumns[i]->getAircraft()->getId()
				<< " lof id " << _betterColumns[i]->getId() << endl;
		}
		*/

		//stable_sort(_betterColumns.begin(), _betterColumns.end(), Lof::compareByAircraft);		//* ȷ��multi-threadÿ�μӽ�master problem��lof˳��һ��
		//sort(_betterColumns.begin(), _betterColumns.end(), Lof::compareByAircraft);
		for (int i = 0; i < Util::threadSize; i++)
		{
			sort(_betterColumns[i].begin(), _betterColumns[i].end(), Lof::compareByAircraft);
		}

		/*
		//���Զ�Lof����
		cout << "after sort _betterColumns: " << endl;
		for(int i = 0; i < _betterColumns.size(); i++)
		{
			cout << "aircraft id " << _betterColumns[i]->getAircraft()->getId()
				<< " lof id " << _betterColumns[i]->getId() << endl;
		}
		*/

		//addColumns(_betterColumns);
		//_initColumns.insert(_initColumns.end(), _betterColumns.begin(), _betterColumns.end());
		for (int i = 0; i < Util::threadSize; i++)
		{
			addColumns(_betterColumns[i]);
			_initColumns.insert(_initColumns.end(), _betterColumns[i].begin(), _betterColumns[i].end());
		}

		/*
		cout << "############### MEMORY TEST ###############" << endl;
		cout << "_initColumns.size() is " << _initColumns.size() << endl;

		if (_initColumns.size() > 120000)
		{
			cout << "############### MEMORY TEST ENDS ###############" << endl;
			exit(0);
		}
		*/

		st = clock();
		solve();
		et = clock();
		_lpTime += et - st;
		cout << "# Time for MP_"<<count<<": "<< (et - st) / CLK_TCK << " seconds" << endl;
		cout<<" ********************* END LP SOLUTION "<< count << " *********************"<<endl<<endl;

		count++;

		//if (count > 5) exit(0); //*DEBUG dual by CPLEX

		//_betterColumns.clear();
		clearBetterColumnsAll();
		st = clock();
		findNewColumns();
		et = clock();
		_spTime += et - st;
		cout << "# Time for SP_" << count <<": " << (et - st) / CLK_TCK << " seconds" << endl;

		cout << "# Total time for LP: " << _lpTime / CLK_TCK << " seconds" << endl;
		cout << "# Total time for SP: " << _spTime / CLK_TCK << " seconds" << endl;
	}

	st = clock();
	lofListSoln = solveIP();
	et = clock();
	_ipTime = et - st;
	cout << "# Time for IP_: " << (et - st) / CLK_TCK << " seconds" << endl;

	return lofListSoln;
}

void Model::clearBetterColumnsAll()
{
	for(int i = 0; i < Util::threadSize; i++)
	{
		_betterColumns[i].clear();
	}
}

bool Model::hasBetterColumn()
{
	for(int i = 0; i < Util::threadSize; i++)
	{
		if (!_betterColumns[i].empty())
			return true;
	}
	return false;
}

void Model::solve()
{
	//_solver.end();
	//_solver = IloCplex(_model);

	char str[16] ;
	itoa(_count,str,10);
	_count++;
	string name = "recovery_" + string(str) + ".lp";

	//_solver.exportModel("test.lp");
	//_solver.exportModel(name.c_str());

	_solver.setParam(IloCplex::RootAlg, IloCplex::Barrier); //* �������LP���㷨����Barrier Scenario1����������CG��������
	_solver.setParam(IloCplex::BarCrossAlg, IloCplex::NoAlg);
	_solver.solve();

	cout << endl;
	cout << "Number of leg variables is: " << _legVar.getSize() << endl;
	cout << "Number of lof variables is: " << _lofVar.getSize() << endl;
	cout << "Number of selection constraints is: " << _selectRng.getSize() << endl;
	cout << "Number of cover constraints is: " << _coverRng.getSize() << endl;
	cout << endl;

	cout << "Solution status: " << _solver.getStatus() << endl;
	cout << "Optimal value: " << _solver.getObjValue() << endl;

	_lpObjVal = _solver.getObjValue();

	_tolerance = _solver.getParam(IloCplex::Param::MIP::Tolerances::Integrality);  /// GET parameter, tolerance
	// cout << "_tolerance" << _tolerance << endl;

	//* get leg dual
	IloNumArray legDual(_env);
	_solver.getDuals(legDual, _coverRng);

	//* set leg dual
	for (int i = 0; i < _legList.size(); i++)
	{
		if (i != _legList[i]->getId())
		{
			cout << "Error, leg index mismatch when get dual" << endl;
			exit(0);
		}
		//cout << "leg " << i << " dual equals " << legDual[i] << endl;
		_legList[i]->setDual(legDual[i]);
	}

	//* get aircraft dual
	IloNumArray aircraftDual(_env);
	_solver.getDuals(aircraftDual, _selectRng);

	//* set aircraft dual
	for (int i = 0; i < _aircraftList.size(); i++)
	{
		if (i != _aircraftList[i]->getId())
		{
			cout << "Error, aircraft index mismatch when get dual" << endl;
			exit(0);
		}
		//cout << "aircraft " << i << " dual equals " << aircraftDual[i] << endl;
		_aircraftList[i]->setDual(aircraftDual[i]);
	}
}

vector<Lof* > Model::solveIP()
{
	cout<<" ********************* FINAL IP SOLUTION *********************"<<endl;

	_model.add(IloConversion(_env, _lofVar, ILOBOOL));
	_model.add(IloConversion(_env, _legVar, ILOBOOL));
	//* _model.add(IloConversion(_env, _grdArcVar, ILOINT));
	//* _model.add(IloConversion(_env, _terminalVar, ILOINT));

	//_solver = IloCplex(_model);

	//_solver.exportModel("recovery.lp");

	_solver.solve();

	cout << endl;
	cout << "Number of leg variables is: " << _legVar.getSize() << endl;
	cout << "Number of lof variables is: " << _lofVar.getSize() << endl;
	cout << "Number of selection constraints is: " << _selectRng.getSize() << endl;
	cout << "Number of cover constraints is: " << _coverRng.getSize() << endl;
	cout << endl;

	_solver.out() << "Solution status: " << _solver.getStatus() << endl;
	_solver.out() << "Optimal value: " << _solver.getObjValue() << endl;

	_ipObjVal = _solver.getObjValue();
	_lofSize = _lofVar.getSize();

	IloNumArray lofVarSoln = IloNumArray(_env);                 /// IloNumArray lofVarSoln(_env)?

	_solver.getValues(lofVarSoln, _lofVar);
	//cout << "_lofVar solution is " << lofVarSoln << endl;		//*
	vector<Lof *> lofListSoln;

	if (_initColumns.size() != 0)		//*
	{
		for (int i = 0; i < lofVarSoln.getSize(); i++)
		{
			//* if (lofVarSoln[i] == 1 )
			if (lofVarSoln[i] <= 1.0001 && lofVarSoln[i] >= 0.9999 )	//*
			{
				//	InitColums[i]->print();
				lofListSoln.push_back(_initColumns[i]);		//*
				_initColumns[i]->print();					//*
			}
		}
	}
	/*
	else
	{
		for (int i = 0; i < lofVarSoln.getSize(); i++)    /// ΪʲôInitColums.size()�п��ܵ���0�أ�duplicated model IP
		{
			if (lofVarSoln[i] == 1)
			{
				lofListSoln.push_back(_lofList[i]);
				_lofList[i]->print();
			}
		}
	}
	*/

	/*
	IloNumArray grdArcVarSoln = IloNumArray(_env);
	_solver.getValues(grdArcVarSoln, _grdArcVar);
	*/

	IloNumArray legVarSoln = IloNumArray(_env);
	_solver.getValues(legVarSoln, _legVar);
	//cout << "_legVar solution is " << legVarSoln << endl;		//*
	//cout << "_lofVar solution is " << lofVarSoln << endl;		//*

	/*
	IloNumArray terminalVarSoln = IloNumArray(_env);
	_solver.getValues(terminalVarSoln, _terminalVar);
	*/

	cout<<endl<<" ********************* END FINAL IP SOLUTION *********************"<<endl;

	return lofListSoln;
}

//Lof* Model::findNewOneColumn(Aircraft* aircraft)
//{
//	// ����_legList[0]�ǳ�ʼleg // ����multi label
//	// �ֹ�����_legList[0]��subNodeList
//	/*
//	SubNode* subNode = new SubNode(_legList[0], NULL, 15, 10);
//	//SubNode* subNode = new SubNode(_legList[0], NULL, 100, 1);
//	_legList[0]->pushSubNode(subNode);
//
//	subNode = new SubNode(_legList[0], NULL, 14, 11);
//	//subNode = new SubNode(_legList[0], NULL, 50, 5);
//	_legList[0]->pushSubNode(subNode);
//
//	subNode = new SubNode(_legList[0], NULL, 13, 12);
//	//subNode = new SubNode(_legList[0], NULL, 10, 10);
//	_legList[0]->pushSubNode(subNode);
//	*/
//
//	//* ��ʼ����aircraft dep airport�ϵ�flight, i.e. nodeCost
//	vector<Leg*> depLegList;
//	depLegList = aircraft->getDepStation()->getDepLegList();
//	for (int k = 0; k < depLegList.size(); k++)
//	{
//		edgeProcessFlt(depLegList[k], aircraft);
//	}
//
//	//* ��ʼ����aircraft dep airport�ϵ�maint, i.e. nodeCost
//	vector<Leg*> depMaintList;
//	depMaintList = aircraft->getDepStation()->getMaintList();
//	for (int k = 0; k < depMaintList.size(); k++)
//	{
//		edgeProcessMaint(depMaintList[k], aircraft);
//	}
//
//	for(int i = 0; i < _topOrderList.size(); i++)
//	{ //* check each node in topological order, to do relax operation
//		for(int j = 0; j  < _topOrderList[i]->getNextLegList().size(); j++)
//		{
//			Leg * thisLeg = _topOrderList[i];
//			Leg * nextLeg = _topOrderList[i]->getNextLegList()[j];
//
//			// ����insertSubNode
//			// ����edgeProcessFltFlt
//			
//			/*
//			cout << "before edgeProcessFlt" << endl;
//			cout << "lg0 is " << endl;
//			thisLeg->print();
//			cout << "lg1 is " << endl;
//			nextLeg->print();
//			*/
//
//			if (!thisLeg->isMaint() && !nextLeg->isMaint())			// thisLeg is flight; nextLeg is flight
//			{
//				edgeProcessFltFlt(thisLeg, nextLeg, aircraft);
//			}
//
//			if (!thisLeg->isMaint() && nextLeg->isMaint())			// thisLeg is flight; nextLeg is maintenance
//			{
//				edgeProcessFltMaint(thisLeg, nextLeg, aircraft);
//			}
//			
//			if (thisLeg->isMaint() && !nextLeg->isMaint())			// thisLeg is maint; nextLeg is flight
//			{
//				edgeProcessMaintFlt(thisLeg, nextLeg, aircraft);
//			}
//
//			if (thisLeg->isMaint() && nextLeg->isMaint())			// thisLeg is maint; nextLeg is maint
//			{
//				edgeProcessMaintMaint(thisLeg, nextLeg, aircraft);
//			}
//
//			/*
//			cout << "after edgeProcessFlt" << endl;
//			cout << "lg0 is " << endl;
//			thisLeg->print();
//			cout << "lg1 is " << endl;
//			nextLeg->print();
//			*/
//		}
//	}
//
//	/*
//	cout << "****** information of all legs ******" << endl;
//	for (int i = 0; i < _topOrderList.size(); i++) // ��ӡ���
//	{
//		_topOrderList[i]->print();
//		cout << endl;
//	}
//	*/
//
//	//* ������а���aircraft�յ������flight/maint
//	//* �ҳ�flight/maint��subNodeList
//	//* �ҳ�����subNodeList��subNodeCost��С��
//
//	SubNode* minCostSubNode= NULL;
//	double minCost = DBL_MAX;
//
//	vector<Leg* > arrLegList;
//	arrLegList = aircraft->getArrStation()->getArrLegList();
//
//	for (int i = 0; i < arrLegList.size(); i++)
//	{
//		vector<SubNode* > subNodeList = arrLegList[i]->getSubNodeList();
//		for (int j = 0; j < subNodeList.size(); j++)
//		{
//			if (subNodeList[j]->getSubNodeCost() < minCost)
//			{
//				minCostSubNode = subNodeList[j];
//				minCost = subNodeList[j]->getSubNodeCost();
//			}
//		}
//	}
//
//	vector<Leg* > arrMaintList;
//	arrMaintList = aircraft->getArrStation()->getMaintList();
//
//	for (int i = 0; i < arrMaintList.size(); i++)
//	{
//		vector<SubNode* > subNodeList = arrMaintList[i]->getSubNodeList();
//		for (int j = 0; j < subNodeList.size(); j++)
//		{
//			if (subNodeList[j]->getSubNodeCost() < minCost)//##ʲôʱ��set�ģ���
//			{
//				minCostSubNode = subNodeList[j];
//				minCost = subNodeList[j]->getSubNodeCost();
//			}
//		}
//	}
//
//	/* ���ǵ�max delay��Լ������Ҫ����Ƿ���feasible LoF����aircraft��dep��arr airport */
//	if (minCostSubNode == NULL)
//	{
//		cout << "Warning, subproblem found no feasible LoF." << endl;
//
//		// reset����leg��parent, nodeCost, operDepTime, operArrTime
//		for (int i = 0; i < _legList.size(); i++)
//		{
//			_legList[i]->resetLeg();
//		}
//
//		/* cout << "****** information of all legs ******" << endl;
//		for (int i = 0; i < _topOrderList.size(); i++) // ��ӡ���
//		{
//			_topOrderList[i]->print();
//			cout << endl;
//		}*/
//
//		return NULL;
//	}
//
//	// ���reduced cost�Ƿ�С����
//	cout << "reduced cost by subproblem aircraft " << aircraft->getId() << " is: " << minCost - aircraft->getDual() << endl;
//	
//	if (minCost - aircraft->getDual() >= -0.0001)
//	{
//		// reset����leg��parent, nodeCost, operDepTime, operArrTime
//		for (int i = 0; i < _legList.size(); i++)
//		{
//			_legList[i]->resetLeg();//##��һ��aircraftǰҪ�����ʷ��¼
//		}
//		// cout << "reduced cost >= 0! " << endl;
//		return NULL;
//	}
//
//	// ѡ����subNode����stack�� //
//	stack<SubNode*> subNodeSelect;
//	SubNode* tempSubNode = minCostSubNode;
//
//	while (tempSubNode != NULL)
//	{
//		subNodeSelect.push(tempSubNode);
//		tempSubNode = tempSubNode->getParentSubNode();
//	}
//
//	/* ����LoF, �������OperLeg */
//	Leg* tempLeg = NULL;
//	OperLeg * tempOperLeg = NULL;
//	Lof* newLof = new Lof();
//	newLof->setAircraft(aircraft); //* ����lof��aircraft
//
//	while (subNodeSelect.size() > 0)
//	{
//		tempSubNode = subNodeSelect.top();
//		tempLeg = tempSubNode->getLeg();
//		tempOperLeg = new OperLeg(tempLeg, aircraft);
//
//		tempOperLeg->setOpDepTime(tempSubNode->getOperDepTime()); //* set operational dep time for operLeg
//		tempOperLeg->setOpArrTime(tempSubNode->getOperArrTime()); //* set operational arr time for operLeg
//
//		newLof->pushLeg(tempOperLeg);
//
//		subNodeSelect.pop();
//	}
//
//	newLof->computeLofCost(); //* ����lof��cost (delay + swap), ������lof��_cost��
//	newLof->computeReducedCost(); //* ������Ӧ�ú�minCost - aircraft->getDual() һ����������lof��_reducedCost��
//
//	//* ���ǵ�CPLEX���������Բ�ֵ �����Ǿ��Բ�ֵ
//	double error = (newLof->getReducedCost()) - (minCost - aircraft->getDual());
//	error = abs(error) / min(abs(newLof->getReducedCost()), abs(minCost - aircraft->getDual()));
//	if (error > 0.0001)
//	{
//		cout << "newLof->getReducedCost() = " << newLof->getReducedCost() << endl;
//		cout << "minCost - aircraft->getDual() = " << minCost - aircraft->getDual() << endl << endl;
//
//		cout << "Error, subproblem reduced cost and minCost not match" << endl;
//
//		//cout << "newLof->getCost = " << newLof->getCost() << endl;
//		cout << "minCost is = " << minCost << endl;
//		cout << "aircraft->getDual() = " << aircraft->getDual() << endl;
//
//		//cout << "accumulated delay is " << minCostLeg->getAccDelay() / 60.0 << " mins" << endl;
//		//cout << "accumulated dual is " << minCostLeg->getAccDual() << endl;
//
//		newLof->print();
//		cout << "******* dual of legs are: *******" << endl;
//		vector<OperLeg* > lofOperLegList = newLof->getLegList();
//		for (int i = 0; i < newLof->getSize(); i++)
//		{
//			cout << "dual of leg " << i << " is " << lofOperLegList[i]->getLeg()->getDual() << endl;
//		}
//
//		exit(0);
//	}
//
//	//newLof->print();
//	/* 
//	cout << "****** number of subNodes of all legs ******" << endl;
//	for (int i = 0; i < _legList.size(); i++)
//	{
//		cout << "leg " << i << " has " << _legList[i]->getSubNodeList().size() << " subNodes" << endl;
//	}
//	*/ 
//
//	/* �����Ҫ����leg��parent, nodeCost, operDepTime, operArrTime */
//	for (int i = 0; i < _legList.size(); i++)
//	{
//		_legList[i]->resetLeg();
//	}
//
//	//newLof->print();
//
//	/*
//	cout << "****** number of subNodes of all legs ******" << endl;
//	for (int i = 0; i < _legList.size(); i++)
//	{
//		cout << "leg " << i << " has " << _legList[i]->getSubNodeList().size() << " subNodes" << endl;
//	}
//	*/
//
//	return newLof;
//
//	/*
//	cout << "****** information of all legs ******" << endl;
//	for (int i = 0; i < _topOrderList.size(); i++) // ��ӡ���
//	{
//		_topOrderList[i]->print();
//		cout << endl;
//	}
//	*/
//}

void Model::findNewColumns(Aircraft * aircraft,int threadIndex)
{

	//* ��ʼ����aircraft dep airport�ϵ�flight, i.e. nodeCost
	vector<Leg*> depLegList;
	depLegList = aircraft->getDepStation()->getDepLegList();
	for (int k = 0; k < depLegList.size(); k++)
	{
		edgeProcessFlt(depLegList[k], aircraft, threadIndex);
	}

	//* ��ʼ����aircraft dep airport�ϵ�maint, i.e. nodeCost
	vector<Leg*> depMaintList;
	depMaintList = aircraft->getDepStation()->getMaintList();
	for (int k = 0; k < depMaintList.size(); k++)
	{
		edgeProcessMaint(depMaintList[k], aircraft, threadIndex);
	}

	for (int i = 0; i < _topOrderList.size(); i++)
	{ //* check each node in topological order, to do relax operation
		Leg * thisLeg = _topOrderList[i];
		for (int j = 0; j < thisLeg->getNextLegList().size(); j++)
		{
			Leg * nextLeg = thisLeg->getNextLegList()[j];

			if (!thisLeg->isMaint() && !nextLeg->isMaint())			// thisLeg is flight; nextLeg is flight
			{
				edgeProcessFltFlt(thisLeg, nextLeg, aircraft, threadIndex);//##Ѱ��·��ʱʹ�õ�edge cost ����delay,swap,flight dual
			}

			if (!thisLeg->isMaint() && nextLeg->isMaint())			// thisLeg is flight; nextLeg is maintenance
			{
				edgeProcessFltMaint(thisLeg, nextLeg, aircraft, threadIndex);
			}

			if (thisLeg->isMaint() && !nextLeg->isMaint())			// thisLeg is maint; nextLeg is flight
			{
				edgeProcessMaintFlt(thisLeg, nextLeg, aircraft, threadIndex);
			}

			if (thisLeg->isMaint() && nextLeg->isMaint())			// thisLeg is maint; nextLeg is maint
			{
				edgeProcessMaintMaint(thisLeg, nextLeg, aircraft, threadIndex);
			}

		}
	}

	vector<SubNode* > tmpSubNodeList;

	vector<Leg* > arrLegList;
	arrLegList = aircraft->getArrStation()->getArrLegList();

	for (int i = 0; i < arrLegList.size(); i++)
	{
		for (auto& subNode : arrLegList[i]->getSubNodeList(threadIndex))
		{
			tmpSubNodeList.push_back(subNode);
		}
	}

	vector<Leg* > arrMaintList;
	arrMaintList = aircraft->getArrStation()->getMaintList();

	for (int i = 0; i < arrMaintList.size(); i++)
	{
		for (auto& subNode : arrMaintList[i]->getSubNodeList(threadIndex))
		{
			tmpSubNodeList.push_back(subNode);
		}

	}

	if (tmpSubNodeList.size() == 0)
	{
		cout << "Warning, subproblem found no feasible LoF." << endl;

		// reset����leg��parent, nodeCost, operDepTime, operArrTime
		for (int i = 0; i < _legList.size(); i++)
		{
			_legList[i]->resetLeg(threadIndex);
		}

		return ;
	}

	sort(tmpSubNodeList.begin(),tmpSubNodeList.end(), SubNode::cmpByCost);
	//stable_sort(tmpSubNodeList.begin(),tmpSubNodeList.end(), SubNode::cmpByCost);


	/* ���ǵ�max delay��Լ������Ҫ����Ƿ���feasible LoF����aircraft��dep��arr airport */


	if (tmpSubNodeList.front()->getSubNodeCost() - aircraft->getDual() >= -0.0001)
	{
		//cout << "reduced cost by subproblem aircraft " << aircraft->getId() << " is: " << tmpSubNodeList.front()->getSubNodeCost() - aircraft->getDual() << endl;
		// reset����leg��parent, nodeCost, operDepTime, operArrTime
		for (int i = 0; i < _legList.size(); i++)
		{
			_legList[i]->resetLeg(threadIndex);//##��һ��aircraftǰҪ�����ʷ��¼
		}
		// cout << "reduced cost >= 0! " << endl;
		return ;
	}

	int tmp_count = 0;
	for (auto& subNode : tmpSubNodeList)
	{
		//if (tmp_count < NEWAMOUNT)
		if (tmp_count < Util::newamount)
		{
			//if (subNode->getSubNodeCost() - aircraft->getDual() < CPLEXERROR)
			if (subNode->getSubNodeCost() - aircraft->getDual() < -0.0001)
			{
				//cout << "reduced cost by subproblem aircraft " << aircraft->getId() << " is: " << subNode->getSubNodeCost() - aircraft->getDual() << endl;
				// ѡ����subNode����stack�� //
				stack<SubNode*> subNodeSelect;
				SubNode* tempSubNode = subNode;
				while (tempSubNode != NULL)
				{
					subNodeSelect.push(tempSubNode);
					tempSubNode = tempSubNode->getParentSubNode();
				}

				/* ����LoF, �������OperLeg */
				Leg* tempLeg = NULL;
				OperLeg * tempOperLeg = NULL;
				Lof* newLof = new Lof();
				newLof->setAircraft(aircraft); //* ����lof��aircraft

				while (subNodeSelect.size() > 0)
				{
					tempSubNode = subNodeSelect.top();
					tempLeg = tempSubNode->getLeg();
					tempOperLeg = new OperLeg(tempLeg, aircraft);

					tempOperLeg->setOpDepTime(tempSubNode->getOperDepTime()); //* set operational dep time for operLeg
					tempOperLeg->setOpArrTime(tempSubNode->getOperArrTime()); //* set operational arr time for operLeg

					newLof->pushLeg(tempOperLeg);

					subNodeSelect.pop();
				}

				newLof->computeLofCost(); //* ����lof��cost (delay + swap), ������lof��_cost��
				newLof->computeReducedCost(); //* ������Ӧ�ú�minCost - aircraft->getDual() һ����������lof��_reducedCost��


				//* ���ǵ�CPLEX���������Բ�ֵ �����Ǿ��Բ�ֵ
				double error = (newLof->getReducedCost()) - (subNode->getSubNodeCost() - aircraft->getDual());
				error = abs(error) / min(abs(newLof->getReducedCost()), abs(subNode->getSubNodeCost() - aircraft->getDual()));
				if (error > 0.0001)
				{
					cout << "newLof->getReducedCost() = " << newLof->getReducedCost() << endl;
					cout << "minCost - aircraft->getDual() = " << subNode->getSubNodeCost() - aircraft->getDual() << endl << endl;

					cout << "Error, subproblem reduced cost and minCost not match" << endl;


					cout << "minCost is = " << subNode->getSubNodeCost() << endl;
					cout << "aircraft->getDual() = " << aircraft->getDual() << endl;


					newLof->print();
					cout << "******* dual of legs are: *******" << endl;
					vector<OperLeg* > lofOperLegList = newLof->getLegList();
					for (int i = 0; i < newLof->getSize(); i++)
					{
						cout << "dual of leg " << i << " is " << lofOperLegList[i]->getLeg()->getDual() << endl;
					}

					break;
					//exit(0);
				}

				//_betterColumns.push_back(newLof);
				_betterColumns[threadIndex].push_back(newLof);
				++tmp_count;

			}
		}
		else
		{
			break;
		}
	}


	/* �����Ҫ����leg��parent, nodeCost, operDepTime, operArrTime */
	for (int i = 0; i < _legList.size(); i++)
	{
		_legList[i]->resetLeg(threadIndex);
	}

	return ;
}

unsigned int Model::findNewColumnsParallel(void * theNr)
{
	pair<int, int> index = *(pair<long, long> *)(theNr);
	//int threadIndex = index.first / (_aircraftList.size() / THREADSIZE + 1);//�̱߳��
	int threadIndex = index.first / (_aircraftList.size() / Util::threadSize + 1);//�̱߳��

	for (int i = index.first; i < index.second; ++i)
	{
		findNewColumns(_aircraftList[i], threadIndex);
	}
	return 0;
}

void Model::edgeProcessFlt(Leg* nextLeg, Aircraft* aircraft, int threadIndex)
{
	if (nextLeg->isMaint())
	{
		cout << "Error, input of edgeProcessFlt must be flight." << endl;
		exit(0);
	}

	time_t delay = 0;
	double edgeCost = 0;
	// ����Ƿ������aircraft��availabe time���delay
	if (aircraft->getStartTime() > nextLeg->getDepTime())
	{
		delay = aircraft->getStartTime() - nextLeg->getDepTime();
	}

	time_t delay2 = delayByAirportClose(nextLeg, delay); // compute delay by airport closure

	delay = delay + delay2; // overall delay

	//* check maximum delay satisfied
	if (delay > Util::maxDelayTime)
		return;

	//* ����Ƿ�ᳬ��aircraft endTime
	if (nextLeg->getArrTime() + delay > aircraft->getEndTime())
		return;

	//edgeCost = delay * Util::w_fltDelay - nextLeg->getDual();
	edgeCost = delay /60.0 * Util::w_fltDelay - nextLeg->getDual();

	//* swap cost
	if (nextLeg->getAircraft() != aircraft)
		edgeCost += Util::w_fltSwap;

	// ���nextLeg��subNodeList�Ƿ�Ϊ��
	// ��ʼ��starting legʱleg��subNodeList��ȻΪ��
	if (!nextLeg->getSubNodeList(threadIndex).empty())
	{
		cout << "Error, initial leg's subNodeList must be empty" << endl;
		exit(0);
	}

	SubNode* newSubNode = new SubNode(nextLeg, NULL, edgeCost, delay);
	
	if (!nextLeg->insertSubNode(newSubNode, threadIndex))
	{
		cout << "Error, initial relaxation must happen" << endl;
		exit(0);
	}
}

void Model::edgeProcessMaint(Leg* nextLeg, Aircraft* aircraft,int threadIndex)
{
	if (!nextLeg->isMaint())
	{
		cout << "Error, input of edgeProcessMaint must be maintenance." << endl;
		exit(0);
	}

	double edgeCost = 0;
	if (nextLeg->getAircraft() == aircraft) // ���nextLeg��aircraftƥ��
	{ 
		if (aircraft->getStartTime() > nextLeg->getDepTime()) // ���nextLeg��maintenance��Ҫ��delay
		{ 
			/* do nothing, since maintenance cannot be delayed */
		}
		else
		{
			//* ����Ƿ�ᳬ��aircraft endTime
			if (nextLeg->getArrTime() > aircraft->getEndTime())
				return;

			//edgeCost += 0 - nextLeg->getDual();
			edgeCost = 0 - nextLeg->getDual();

			// ���nextLeg��subNodeList�Ƿ�Ϊ��
			// ��ʼ��starting legʱleg��subNodeList��ȻΪ��
			if (!nextLeg->getSubNodeList(threadIndex).empty())
			{
				cout << "Error, initial leg's subNodeList must be empty" << endl;
				exit(0);
			}

			SubNode* newSubNode = new SubNode(nextLeg, NULL, edgeCost, 0);

			if (!nextLeg->insertSubNode(newSubNode, threadIndex))
			{
				cout << "Error, initial relaxation must happen" << endl;
				exit(0);
			}
		}
	}
	else // ���nextLeg��aircraft��ƥ��
	{
		// do nothing
		/*
		nextLeg->setNodeCost(DBL_MAX);
		nextLeg->setParent(NULL);
		*/
	}
}

//* helper function for findNewOneColumn *//
void Model::edgeProcessFltFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft,int threadIndex)
{
	vector<SubNode*> subNodeList = thisLeg->getSubNodeList(threadIndex);
	for(int i = 0; i < subNodeList.size(); i++)
	{
		edgeProcessFltFlt(subNodeList[i], nextLeg, aircraft, threadIndex);
	}
}

//* helper function for edgeProcessFltFlt *//
void Model::edgeProcessFltFlt(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft,int threadIndex)
{
	time_t delay = 0;
	double edgeCost = 0;

	/*
	cout << "##### begin of edgeProcess SubNode #####" << endl;
	cout << "previous subNode is " << endl;
	subNode->print();
	*/

	delay = computeFlightDelay(subNode, nextLeg);

	//edgeCost = delay * Util::w_fltDelay - nextLeg->getDual();
	edgeCost = delay / 60.0 * Util::w_fltDelay - nextLeg->getDual(); //* delayת���ɷ���

	//* check maximum delay satisfied
	if (delay > Util::maxDelayTime)
		return;

	//* ����Ƿ�ᳬ��aircraft endTime
	if (nextLeg->getArrTime() + delay > aircraft->getEndTime())
		return;

	//* swap cost
	if (nextLeg->getAircraft() != aircraft)
		edgeCost += Util::w_fltSwap;

	// �½�SubNode, ����Ƿ��ܲ���nextLeg��subNodeList
	// ���newSubNode�����κ�nextLeg���е�subNode dominate, ����
	// ���newSubNode dominate�κ�nextLeg���е�subNode, ɾ�������е�subNode, newSubNode����nextLeg��subNodeList
	SubNode* newSubNode = new SubNode(nextLeg, subNode, subNode->getSubNodeCost() + edgeCost, delay);

	/*
	cout << "new subNode is " << endl;
	subNode->print();
	cout << "##### end of edgeProcess SubNode #####" << endl;
	*/ 

	if (!nextLeg->insertSubNode(newSubNode, threadIndex))
	{
		delete newSubNode;
	}
}

//* helper function for findNewOneColumn *//
void Model::edgeProcessFltMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft,int threadIndex)
{
	vector<SubNode*> subNodeList = thisLeg->getSubNodeList(threadIndex);
	for (int i = 0; i < subNodeList.size(); i++)
	{
		edgeProcessFltMaint(subNodeList[i], nextLeg, aircraft, threadIndex);
	}
}


// helper function for edgeProcessFltMaint
void Model::edgeProcessFltMaint(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft,int threadIndex)
{
	time_t delay = 0;
	double edgeCost = 0;
	
	if (nextLeg->getAircraft() == aircraft)
	{ // ���nextLeg��aircraftƥ��
		if (subNode->getOperArrTime() > nextLeg->getDepTime())
		{ // ���nextLeg��maintenance��Ҫ��delay
			/* do nothing, since maintenance cannot be delayed*/
		}
		else
		{
			//* ����Ƿ�ᳬ��aircraft endTime
			if (nextLeg->getArrTime() > aircraft->getEndTime())
				return;

			edgeCost = 0 - nextLeg->getDual();

			SubNode* newSubNode = new SubNode(nextLeg, subNode, subNode->getSubNodeCost() + edgeCost, delay);

			if (!nextLeg->insertSubNode(newSubNode, threadIndex))
			{
				delete newSubNode;
			}
		}
	}else { // ���nextLeg��aircraft��ƥ��
		// do nothing
		// ����nextLeg��subNodeList���subNode
	}
}

void Model::edgeProcessMaintFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft,int threadIndex)
{
	vector<SubNode*> subNodeList = thisLeg->getSubNodeList(threadIndex);
	for (int i = 0; i < subNodeList.size(); i++)
	{
		edgeProcessMaintFlt(subNodeList[i], nextLeg, aircraft, threadIndex);
	}
}

void Model::edgeProcessMaintFlt(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft,int threadIndex)
{
	time_t delay = 0;
	double edgeCost = 0;

	Leg* thisLeg = subNode->getLeg();

	if (thisLeg->getAircraft() == aircraft) { // ���thisLeg��maitenance��aircraftƥ��
		
		delay = computeFlightDelay(subNode, nextLeg);
		//edgeCost = delay * Util::w_fltDelay - nextLeg->getDual();
		edgeCost = delay / 60.0 * Util::w_fltDelay - nextLeg->getDual();

		//* check maximum delay satisfied
		if (delay > Util::maxDelayTime)
			return;

		//* ����Ƿ�ᳬ��aircraft endTime
		if (nextLeg->getArrTime() + delay > aircraft->getEndTime())
			return;

		//* swap cost
		if (nextLeg->getAircraft() != aircraft)
			edgeCost += Util::w_fltSwap;

		SubNode* newSubNode = new SubNode(nextLeg, subNode, subNode->getSubNodeCost() + edgeCost, delay);

		if (!nextLeg->insertSubNode(newSubNode, threadIndex))
		{
			delete newSubNode;
		}
	}else { // ���thisLeg��maitenance��aircraft��ƥ��
		if (!thisLeg->getSubNodeList(threadIndex).empty()) { // ���thisLeg��maitenance��aircraft��ƥ�䣬��subNodeList�ǿ�
			cout << "Error, maintenance and aircraft mismatch; subNodeList of maintenance must be empty!" << endl;
			exit(0);
		}
	}
}

void Model::edgeProcessMaintMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft,int threadIndex)
{
	vector<SubNode*> subNodeList = thisLeg->getSubNodeList(threadIndex);
	for (int i = 0; i < subNodeList.size(); i++)
	{
		edgeProcessMaintMaint(subNodeList[i], nextLeg, aircraft, threadIndex);
	}
}

void Model::edgeProcessMaintMaint(SubNode* subNode, Leg* nextLeg, Aircraft* aircraft,int threadIndex)
{
	time_t delay = 0;
	double edgeCost = 0;

	Leg* thisLeg = subNode->getLeg();

	if (thisLeg->getAircraft() != nextLeg->getAircraft())
	{
		cout << "Error, aircraft of two connected maintenances do not match" << endl;
		exit(0);
	}

	if (nextLeg->isMaint()) { // ���nextLegҲ��maitenance
		if (nextLeg->getAircraft() == aircraft) { // ���aircraft��maintenanceƥ��
			// ���nextLeg�Ƿ�ᱻdelay, schedule�����neighbor��ʱ��Ӧ���Ѿ���֤
			if (subNode->getOperArrTime() > nextLeg->getDepTime()) {
				cout << "Error, maintenance cannot be delayed!" << endl;
				exit(0);
			}

			//* ����Ƿ�ᳬ��aircraft endTime
			if (nextLeg->getArrTime() > aircraft->getEndTime())
				return;

			edgeCost = 0 - nextLeg->getDual();
			
			SubNode* newSubNode = new SubNode(nextLeg, subNode, subNode->getSubNodeCost() + edgeCost, delay);

			if (!nextLeg->insertSubNode(newSubNode, threadIndex))
			{
				delete newSubNode;
			}

		}else { // ���aircraft��maintenance��ƥ��
			if (!thisLeg->getSubNodeList(threadIndex).empty()) { // thisLeg��maint, aircraft��ƥ�䣬node cost�����ܲ����޸Ĺ�
				cout << "Error, thisLeg maintenance and aircraft do not match!" << endl;
				exit(0);
			}

			/*
			nextLeg->setNodeCost(DBL_MAX);
			nextLeg->setParent(NULL);
			*/
		}
	}
	else
	{
		cout << "Error, nextLeg should be maintenance!" << endl;
		exit(0);
	}
}

//* helper function for edgeProcess, to compute delay, consider airport closure *//
time_t Model::computeFlightDelay (SubNode* subNode, Leg* nextLeg)
{
	time_t delay = 0;
	Leg* thisLeg = subNode->getLeg();

	if (nextLeg->isMaint())
	{
		cout << "Error, nextLeg must be flight to compute delay!" << endl;
		exit(0);
	}

	//* �Ȳ�����airport closure������Ƿ���Ҫdelay
	//* �����������thisLeg�Ƿ�Ϊflight
	if (thisLeg->isMaint()) // edgeProcess����ж��Ƿ���aircraftƥ�䣬�������ﲻ�ж�
	{
		if (subNode->getOperArrTime() > nextLeg->getDepTime())
		{
			delay = subNode->getOperArrTime() - nextLeg->getDepTime();
		}
	}
	else // thisLeg is flight
	{
		if (subNode->getOperArrTime() + Util::turnTime > nextLeg->getDepTime())
		{
			delay = subNode->getOperArrTime() + Util::turnTime - nextLeg->getDepTime();
		}
	}

	//* �ٿ���airport closure
	time_t delay2 = delayByAirportClose(nextLeg, delay);

	return delay + delay2;
}

//* helper function for computeFlightDelay *//
time_t Model::delayByAirportClose (Leg* nextLeg, time_t delay)
{
	if (nextLeg->isMaint())
	{
		cout << "Error, nextLeg must be flight to compute delay!" << endl;
		exit(0);
	}

	time_t nextLegDepTime = nextLeg->getDepTime() + delay;
	time_t nextLegArrTime = nextLeg->getArrTime() + delay;

	//* nextLeg��flight, ���nextLeg operDepTime and operArrTime�Ƿ��ܵ����������رյ�Ӱ��
	//* ����ܵ�Ӱ�죬��delay
	time_t delay2 = 0;
	vector<pair<time_t, time_t>> depCloseList;
	depCloseList = nextLeg->getDepStation()->getCloseTimeList();
	vector<pair<time_t, time_t>> arrCloseList;
	arrCloseList = nextLeg->getArrStation()->getCloseTimeList();

	//* ���ܻ��м���depDelay��Ȼ������dep station��close, ������ɽ���arr station��close�����; ��֮��Ȼ
	//* ������Ҫ����
	bool stopFlag1 = 0;
	bool stopFlag2 = 0;

	while ((!stopFlag1 || !stopFlag2))
	{
		stopFlag1 = 1;
		for (int k = 0; k < depCloseList.size(); k++)
		{
			if (nextLegDepTime + delay2 >= depCloseList[k].first &&
				nextLegDepTime + delay2 < depCloseList[k].second)
			{
				delay2 = depCloseList[k].second - nextLegDepTime;
				stopFlag1 = 0;
				break; // airport closureʱ�β��ص�
			}
		}

		//cout << "delay by airport closure delay2 = " << delay2 << endl;
		//cout << "stopFlag1 = " << stopFlag1 << endl;

		stopFlag2 = 1;
		for (int k = 0; k < arrCloseList.size(); k++)
		{
			if (nextLegArrTime + delay2 >= arrCloseList[k].first &&
				nextLegArrTime + delay2 < arrCloseList[k].second)
			{
				delay2 = arrCloseList[k].second - nextLegArrTime;
				stopFlag2 = 0;
				break; // airport closureʱ�β��ص�
			}
		}

		//cout << "stopFlag2 = " << stopFlag2 << endl;
		//cout << "(!stopFlag1) || (!stopFlag2)= " << ((!stopFlag1) || (!stopFlag2)) << endl;
	}

	return delay2;
}
