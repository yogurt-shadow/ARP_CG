#include "Model.h"

Model::Model(vector<Station *> stationList, vector<Aircraft *> aircraftList, vector<Leg *> legList, vector<Leg *> topOrderList):
	_stationList(stationList), _aircraftList(aircraftList), _legList(legList), _topOrderList(topOrderList),
	_dummySource("dummySource")
{
	//* initialize CPLEX objects
	_model = IloModel(_env, "Minimize");
	_obj = IloMinimize(_env);
	_lofVar = IloNumVarArray(_env);
	_legVar = IloNumVarArray(_env);
	_coverRng = IloRangeArray(_env);
	_selectRng = IloRangeArray(_env);
	_solver = IloCplex(_model);
}

int Model::_count = 0;

vector<Lof *> Model::findNewColumns()
{
	vector<Lof* > betterLof;
	Lof* tempLof;

	for (int i = 0; i < _aircraftList.size(); i++)
	{
		tempLof = findNewOneColumn(_aircraftList[i]);
		if (tempLof != NULL)
		{
			betterLof.push_back(tempLof);
		}
	}

	cout << "Number of Better Lofs is " << betterLof.size() << endl << endl;

	/*
	for (int i = 0; i < betterLof.size(); i++)
	{
		betterLof[i]->print();
	}
	*/

	return betterLof;
}

Lof* Model::findNewOneColumn(Aircraft* aircraft)
{
	//* _topOrderList[0]->setNodeCost(0); //* 测试DP计算shortest path,假设_topOrderList[0]是起始航班
	//edgeProcessFlt(_topOrderList[0], aircraft);

	//* 初始化在aircraft dep airport上的flight, i.e. nodeCost
	vector<Leg*> depLegList;
	depLegList = aircraft->getDepStation()->getDepLegList();
	for (int k = 0; k < depLegList.size(); k++)
	{
		edgeProcessFlt(depLegList[k], aircraft);
	}

	//* 初始化在aircraft dep airport上的maint, i.e. nodeCost
	vector<Leg*> depMaintList;
	depMaintList = aircraft->getDepStation()->getMaintList();
	for (int k = 0; k < depMaintList.size(); k++)
	{
		edgeProcessMaint(depMaintList[k], aircraft);
	}

	for(int i = 0; i < _topOrderList.size(); i++)
	{ //* check each node in topological order, to do relax operation
		//int i = 0;
		for(int j = 0; j < _topOrderList[i]->getNextLegList().size(); j++)
		{ //* check the successors of current node (thisLeg)
			Leg * thisLeg = _topOrderList[i];
			Leg * nextLeg = _topOrderList[i]->getNextLegList()[j];

			//* 还需要分别讨论thisLeg, nextLeg是flight或maintenance的情况
			//* 是maintenance的话还要注意是否与飞机匹配
			//* maintenance还不能被delay
			//* 还要考虑由于天气机场关闭的影响

			if (!thisLeg->isMaint() && !nextLeg->isMaint())			// thisLeg is flight; nextLeg is flight
			{
				edgeProcessFltFlt(thisLeg, nextLeg, aircraft);
			}

			if (!thisLeg->isMaint() && nextLeg->isMaint())			// thisLeg is flight; nextLeg is maintenance
			{
				edgeProcessFltMaint(thisLeg, nextLeg, aircraft);
			}		

			if (thisLeg->isMaint() && !nextLeg->isMaint())			// thisLeg is maintenance; nextLeg is flight
			{
				edgeProcessMaintFlt(thisLeg, nextLeg, aircraft);
			}

			if (thisLeg->isMaint() && nextLeg->isMaint())			// thisLeg is maintenance; nextLeg is maintenance
			{
				edgeProcessMaintMaint(thisLeg, nextLeg, aircraft);
			}
		}
	}

	//* 检查所有包含aircraft终点机场的flight/maint，找出nodeCost最小的那个 *//
	Leg* minCostLeg= NULL;
	double minCost = DBL_MAX;

	vector<Leg*> arrLegList;
	arrLegList = aircraft->getArrStation()->getArrLegList();
	for (int k = 0; k < arrLegList.size(); k++)
	{
		//if (arrLegList[k]->getOperArrTime() <= aircraft->getEndTime()) //* 检查是否满足aircraft endTime约束
		//{
			if (arrLegList[k]->getNodeCost() < minCost)
			{
				minCostLeg = arrLegList[k];
				minCost = arrLegList[k]->getNodeCost();
			}
		//}
	}

	vector<Leg*> arrMaintList;
	arrMaintList = aircraft->getArrStation()->getMaintList();
	for (int k = 0; k < arrMaintList.size(); k++)
	{
		//if (arrLegList[k]->getOperArrTime() <= aircraft->getEndTime()) //* 检查机场是否满足aircraft endTime约束
		//{
			if (arrMaintList[k]->getNodeCost() < minCost)
			{
				minCostLeg = arrMaintList[k];
				minCost = arrMaintList[k]->getNodeCost();
			}
		//}
	}

	/* 考虑到max delay的约束，需要检查是否有feasible LoF连接aircraft的dep和arr airport */
	if (minCostLeg == NULL)
	{
		cout << "Warning, subproblem found no feasible LoF." << endl;

		// reset所有leg的parent, nodeCost, operDepTime, operArrTime
		for (int i = 0; i < _legList.size(); i++)
		{
			_legList[i]->resetLeg();
		}

		/* cout << "****** information of all legs ******" << endl;
		for (int i = 0; i < _topOrderList.size(); i++) // 打印检查
		{
			_topOrderList[i]->print();
			cout << endl;
		}*/

		return NULL;
	}

	// 检查reduced cost是否小于零
	cout << "reduced cost by subproblem aircraft " << aircraft->getId() << " is: " << minCost - aircraft->getDual() << endl;
	
	if (minCost - aircraft->getDual() >= -0.0001)
	{
		// reset所有leg的parent, nodeCost, operDepTime, operArrTime
		for (int i = 0; i < _legList.size(); i++)
		{
			_legList[i]->resetLeg();
		}
		// cout << "reduced cost >= 0! " << endl;
		return NULL;
	}
	

	/* 选出的leg存在stack里 */
	stack<Leg*> legSelect;
	Leg* tempLeg = minCostLeg;
	//* while (tempLeg != NULL)
	while (tempLeg != &_dummySource)
	{
		legSelect.push(tempLeg);
		tempLeg = tempLeg->getParent();
	}

	/* 创建LoF, 创建添加OperLeg */
	OperLeg * tempOperLeg = NULL;
	Lof* newLof = new Lof();
	newLof->setAircraft(aircraft); //* 设置lof的aircraft

	while (legSelect.size() > 0)
	{
		tempLeg = legSelect.top();
		tempOperLeg = new OperLeg(tempLeg, aircraft);

		tempOperLeg->setOpDepTime(tempLeg->getOperDepTime()); //* set operational dep time for operLeg
		tempOperLeg->setOpArrTime(tempLeg->getOperArrTime()); //* set operational arr time for operLeg

		newLof->pushLeg(tempOperLeg);

		legSelect.pop();
	}

	newLof->computeLofCost(); //* 计算lof的cost (delay + swap), 更新在lof的_cost里
	newLof->computeReducedCost(); //* 计算结果应该和minCost - aircraft->getDual() 一样，更新在lof的_reducedCost里

	
	//* 考虑到CPLEX的误差，检查相对差值 而不是绝对差值
	double error = (newLof->getReducedCost()) - (minCost - aircraft->getDual());
	error = abs(error) / min(abs(newLof->getReducedCost()), abs(minCost - aircraft->getDual()));
	if (error > 0.0001)
	{
		cout << "newLof->getReducedCost() = " << newLof->getReducedCost() << endl;
		cout << "minCost - aircraft->getDual() = " << minCost - aircraft->getDual() << endl << endl;

		cout << "Error, subproblem reduced cost and minCost not match" << endl;

		//cout << "newLof->getCost = " << newLof->getCost() << endl;
		cout << "minCost is = " << minCost << endl;
		cout << "aircraft->getDual() = " << aircraft->getDual() << endl;

		//cout << "accumulated delay is " << minCostLeg->getAccDelay() / 60.0 << " mins" << endl;
		//cout << "accumulated dual is " << minCostLeg->getAccDual() << endl;

		newLof->print();
		cout << "******* dual of legs are: *******" << endl;
		vector<OperLeg* > lofOperLegList = newLof->getLegList();
		for (int i = 0; i < newLof->getSize(); i++)
		{
			cout << "dual of leg " << i << " is " << lofOperLegList[i]->getLeg()->getDual() << endl;
		}

		exit(0);
	}
	

	/*
	if (!(newLof->getReducedCost() <= minCost - aircraft->getDual() + 0.0001
		&& newLof->getReducedCost() >= minCost - aircraft->getDual() - 0.0001))
	{
		cout << "newLof->getReducedCost() = " << newLof->getReducedCost() << endl;
		cout << "minCost - aircraft->getDual() = " << minCost - aircraft->getDual() << endl;
		cout << "Error, subproblem reduced cost and minCost not match" << endl;
		exit(0);
	}
	*/

	// newLof->print();

	/* 最后需要重置leg的parent, nodeCost, operDepTime, operArrTime */
	for (int i = 0; i < _legList.size(); i++)
	{
		_legList[i]->resetLeg();
	}

	return newLof;

	/*
	// 打印选出的leg
	cout << "****** aircraft ******" << endl;
	aircraft->print();
	cout << endl;

	cout << "****** selected legs ******" << endl;
	while (legSelect.size() > 0)
	{
		legSelect.top()->print();
		cout << endl;
		legSelect.pop();
	}
	*/

	/*
	cout << "****** information of all legs ******" << endl;
	for (int i = 0; i < _topOrderList.size(); i++) // 打印检查
	{
		_topOrderList[i]->print();
		cout << endl;
	}
	*/
	
}

//* helper function for edgeProcess, to compute delay, consider airport closure *//
time_t Model::computeFlightDelay (Leg* thisLeg, Leg* nextLeg)
{
	//cout << "begin of calling computeFlightDealy" << endl;
	time_t delay = 0;
	if (nextLeg->isMaint())
	{
		cout << "Error, nextLeg must be flight to compute delay!" << endl;
		exit(0);
	}
	//* 先不考虑airport closure，检查是否需要delay
	//* 分两种情况，thisLeg是否为flight
	if (thisLeg->isMaint()) // edgeProcess里会判断是否与aircraft匹配，所以这里不判断
	{
		if (thisLeg->getOperArrTime() > nextLeg->getDepTime())
		{
			delay = thisLeg->getOperArrTime() - nextLeg->getDepTime();
		}
	}
	else // thisLeg is flight
	{
		if (thisLeg->getOperArrTime() + Util::turnTime > nextLeg->getDepTime())
		{
			delay = thisLeg->getOperArrTime() + Util::turnTime - nextLeg->getDepTime();
		}
	}

	//* 再考虑airport closure
	time_t delay2 = delayByAirportClose(nextLeg, delay);

	//cout << "end of calling computeFlightDealy" << endl;

	return delay + delay2;
}

time_t Model::delayByAirportClose (Leg* nextLeg, time_t delay)
{
	if (nextLeg->isMaint())
	{
		cout << "Error, nextLeg must be flight to compute delay!" << endl;
		exit(0);
	}

	time_t nextLegDepTime = nextLeg->getDepTime() + delay;
	time_t nextLegArrTime = nextLeg->getArrTime() + delay;

	//* nextLeg是flight, 检查nextLeg operDepTime and operArrTime是否受到天气机场关闭的影响
	//* 如果受到影响，加delay
	time_t delay2 = 0;
	vector<pair<time_t, time_t>> depCloseList;
	depCloseList = nextLeg->getDepStation()->getCloseTimeList();
	vector<pair<time_t, time_t>> arrCloseList;
	arrCloseList = nextLeg->getArrStation()->getCloseTimeList();

	//* 可能会有加了depDelay虽然避免了dep station的close, 但又造成进入arr station的close的情况; 反之亦然
	//* 所以需要迭代
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
				break; // airport closure时段不重叠
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
				break; // airport closure时段不重叠
			}
		}

		//cout << "stopFlag2 = " << stopFlag2 << endl;
		//cout << "(!stopFlag1) || (!stopFlag2)= " << ((!stopFlag1) || (!stopFlag2)) << endl;
	}

	return delay2;
}

//* helper function for findNewOneColumn *//
//* 初始化aircraft在network上可能的起始Maint *//
void Model::edgeProcessMaint(Leg* nextLeg, Aircraft* aircraft)
{
	if (!nextLeg->isMaint())
	{
		cout << "Error, input of edgeProcessFlt must be maintenance." << endl;
		exit(0);
	}

	double edgeCost = 0;
	if (nextLeg->getAircraft() == aircraft) // 如果nextLeg与aircraft匹配
	{ 
		if (aircraft->getStartTime() > nextLeg->getDepTime()) // 如果nextLeg的maintenance需要被delay
		{ 
			/* do nothing, since maintenance cannot be delayed */
		}
		else
		{
			//* 检查是否会超过aircraft endTime
			if (nextLeg->getArrTime() > aircraft->getEndTime())
				return;

			//edgeCost += 0 - nextLeg->getDual();
			edgeCost = 0 - nextLeg->getDual();

			//* relax edge if needed
			if (edgeCost < nextLeg->getNodeCost())
			{
				nextLeg->setNodeCost(edgeCost); // update node cost
				nextLeg->setParent(&_dummySource); //* update parent node

				//nextLeg->setAccDual(nextLeg->getDual());
			}
			else
			{
				cout << "Error, initial nodeCost should be BDL_MAX" << endl;
				exit(0);
			}
		}
	}
	else // 如果nextLeg与aircraft不匹配
	{
		nextLeg->setNodeCost(DBL_MAX);
		nextLeg->setParent(NULL);
	}
}

//* helper function for findNewOneColumn *//
//* 初始化aircraft在network上可能的起始flight *//
void Model::edgeProcessFlt(Leg* nextLeg, Aircraft* aircraft)
{
	if (nextLeg->isMaint())
	{
		cout << "Error, input of edgeProcessFlt must be flight." << endl;
		exit(0);
	}

	time_t delay = 0;
	double edgeCost = 0;
	// 检查是否会由于aircraft的availabe time造成delay
	if (aircraft->getStartTime() > nextLeg->getDepTime())
	{
		delay = aircraft->getStartTime() - nextLeg->getDepTime();
	}
	
	time_t delay2 = delayByAirportClose(nextLeg, delay); // compute delay by airport closure

	delay = delay + delay2; // overall delay
	
	//* check maximum delay satisfied
	if (delay > Util::maxDelayTime)
		return;

	//* 检查是否会超过aircraft endTime
	if (nextLeg->getArrTime() + delay > aircraft->getEndTime())
		return;

	// edgeCost = delay * Util::w_fltDelay - nextLeg->getDual();
	edgeCost = delay /60.0 * Util::w_fltDelay - nextLeg->getDual();

	//* swap cost
	if (nextLeg->getAircraft() != aircraft)
		edgeCost += Util::w_fltSwap;

	if (edgeCost < nextLeg->getNodeCost())
	{
		nextLeg->setNodeCost(edgeCost); // update node cost
		nextLeg->setParent(&_dummySource); // update parent node

		nextLeg->setOperDepTime(nextLeg->getDepTime() + delay);
		nextLeg->setOperArrTime(nextLeg->getArrTime() + delay);

		//nextLeg->setAccDelay(delay);
		//nextLeg->setAccDual(nextLeg->getDual());
		//nextLeg->setDelay(delay);
	}
	else
	{
		cout << "Error, initial nodeCost should be BDL_MAX, relax must happen" << endl;
		exit(0);
	}
}

//* helper function for findNewOneColumn *//
void Model::edgeProcessMaintMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft)
{
	time_t delay = 0;
	double edgeCost = 0;

	if (thisLeg->getAircraft() != nextLeg->getAircraft())
	{
		cout << "Error, aircraft of two connected maintenances do not match" << endl;
		exit(0);
	}

	if (nextLeg->isMaint()) { // 如果nextLeg也是maitenance
		if (nextLeg->getAircraft() == aircraft) { // 如果aircraft与maintenance匹配
			// 检查nextLeg是否会被delay, schedule里查找neighbor的时候应该已经保证
			if (thisLeg->getOperArrTime() > nextLeg->getDepTime()) {
				cout << "Error, maintenance cannot be delayed!" << endl;
				exit(0);
			}

			//* 检查是否会超过aircraft endTime
			if (nextLeg->getArrTime() > aircraft->getEndTime())
				return;

			//edgeCost += 0 - nextLeg->getDual();
			edgeCost = 0 - nextLeg->getDual();
			//* relax edge if needed
			if (thisLeg->getNodeCost() + edgeCost < nextLeg->getNodeCost()) {
				nextLeg->setNodeCost(thisLeg->getNodeCost() + edgeCost); // update node cost
				nextLeg->setParent(thisLeg); // update parent node

				//nextLeg->setAccDelay(thisLeg->getAccDelay());
				//nextLeg->setAccDual(thisLeg->getAccDual() + nextLeg->getDual());
			}

		}else { // 如果aircraft与maintenance不匹配
			if (thisLeg->getNodeCost() != DBL_MAX) { // thisLeg是maint, aircraft不匹配，node cost不可能不被修改过
				/*cout << "thisLeg " << endl;
				thisLeg->print();
				cout << "nextLeg " << endl;
				nextLeg->print();*/
				cout << "Error, thisLeg maintenance and aircraft do not match!" << endl;
				exit(0);
			}

			nextLeg->setNodeCost(DBL_MAX);
			nextLeg->setParent(NULL);
		}
	}
}

//* helper function for findNewOneColumn *//
void Model::edgeProcessMaintFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft)
{
	//cout << "edgeProcessMaintFlt is called" << endl;
	time_t delay = 0;
	double edgeCost = 0;

	if (thisLeg->getAircraft() == aircraft) { // 如果thisLeg的maitenance与aircraft匹配
		//* delay and dual
		/*
		if (thisLeg->getOperArrTime() > nextLeg->getDepTime()) { //* check whether delay is needed
			delay = thisLeg->getOperArrTime() - nextLeg->getDepTime();
		}*/
		delay = computeFlightDelay(thisLeg, nextLeg);
		// edgeCost = delay * Util::w_fltDelay - nextLeg->getDual();
		edgeCost = delay / 60.0 * Util::w_fltDelay - nextLeg->getDual();

		//* check maximum delay satisfied
		if (delay > Util::maxDelayTime)
			return;

		//* 检查是否会超过aircraft endTime
		if (nextLeg->getArrTime() + delay > aircraft->getEndTime())
			return;

		//* swap cost
		if (nextLeg->getAircraft() != aircraft)
			edgeCost += Util::w_fltSwap;

		//cout << "thisLeg->getNodeCost() " << thisLeg->getNodeCost() << endl;
		//cout << "edgeCost " << edgeCost << endl;
		//cout << "nextLeg->getNodeCost() " << nextLeg->getNodeCost() << endl;

		//* relax edge if needed
		if (thisLeg->getNodeCost() + edgeCost < nextLeg->getNodeCost()) {
			nextLeg->setNodeCost(thisLeg->getNodeCost() + edgeCost); // update node cost
			nextLeg->setParent(thisLeg); // update parent node

			/* no need to check whether delay is needed, just add, the previously computed delay might be zero */
			//nextLeg->setOperDepTime(nextLeg->getOperDepTime() + delay);
			nextLeg->setOperDepTime(nextLeg->getDepTime() + delay);
			nextLeg->setOperArrTime(nextLeg->getArrTime() + delay);

			//nextLeg->setAccDelay(thisLeg->getAccDelay() + delay);
			//nextLeg->setAccDual(thisLeg->getAccDual() + nextLeg->getDual());
			//nextLeg->setDelay(delay);
		}
	}else { // 如果thisLeg的maitenance与aircraft不匹配
		if (thisLeg->getNodeCost() != DBL_MAX) { // 如果thisLeg的maitenance与aircraft不匹配，但thisLeg的nodeCost不是DBL_MAX
			cout << "Error, thisLeg maintenance and aircraft do not match!" << endl;
			exit(0);
		}
	}
}

//* helper function for findNewOneColumn *//
void Model::edgeProcessFltMaint(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft)
{
	time_t delay = 0;
	double edgeCost = 0;

	if (nextLeg->getAircraft() == aircraft) { // 如果nextLeg与aircraft匹配
		if (thisLeg->getOperArrTime() > nextLeg->getDepTime()) { // 如果nextLeg的maintenance需要被delay
			/* do nothing, since maintenance cannot be delayed*/
		}else {

			//* 检查是否会超过aircraft endTime
			if (nextLeg->getArrTime() > aircraft->getEndTime())
				return;

			//edgeCost += 0 - nextLeg->getDual();
			edgeCost = 0 - nextLeg->getDual();

			//* relax edge if needed
			if (thisLeg->getNodeCost() + edgeCost < nextLeg->getNodeCost()) {
				nextLeg->setNodeCost(thisLeg->getNodeCost() + edgeCost); // update node cost
				nextLeg->setParent(thisLeg); // update parent node

				//nextLeg->setAccDelay(thisLeg->getAccDelay());
				//nextLeg->setAccDual(thisLeg->getAccDual() + nextLeg->getDual());
			}
		}
	}else { // 如果nextLeg与aircraft不匹配
		nextLeg->setNodeCost(DBL_MAX);
		nextLeg->setParent(NULL);
	}
}

//* helper function for findNewOneColumn *//
void Model::edgeProcessFltFlt(Leg* thisLeg, Leg* nextLeg, Aircraft* aircraft)
{
	time_t delay = 0;
	double edgeCost = 0;
	//* delay and dual
	/*
	if (thisLeg->getOperArrTime() + Util::turnTime > nextLeg->getDepTime()) { //* check whether delay is needed
		delay = thisLeg->getOperArrTime() + Util::turnTime - nextLeg->getDepTime();
	}
	*/
	delay = computeFlightDelay(thisLeg, nextLeg);

	// edgeCost = delay * Util::w_fltDelay - nextLeg->getDual();
	edgeCost = delay / 60.0 * Util::w_fltDelay - nextLeg->getDual(); //* delay转换成分钟

	//* check maximum delay satisfied
	if (delay > Util::maxDelayTime)
		return;

	//* 检查是否会超过aircraft endTime
	if (nextLeg->getArrTime() + delay > aircraft->getEndTime())
		return;

	//* swap cost
	if (nextLeg->getAircraft() != aircraft)
		edgeCost += Util::w_fltSwap;

	// cout << "delay is " << delay << endl; //* 检查delay是否正确
	// cout << "edgeCost is " << edgeCost << endl; //* 检查edgeCost是否正确

	//* relax edge if needed
	if (thisLeg->getNodeCost() + edgeCost < nextLeg->getNodeCost()) {
		nextLeg->setNodeCost(thisLeg->getNodeCost() + edgeCost); // update node cost
		nextLeg->setParent(thisLeg); // update parent node

		/* no need to check whether delay is needed, just add, the previously computed delay might be zero */
		//nextLeg->setOperDepTime(nextLeg->getOperDepTime() + delay); // bug corrected 20161215
		nextLeg->setOperDepTime(nextLeg->getDepTime() + delay);
		nextLeg->setOperArrTime(nextLeg->getArrTime() + delay);

		//nextLeg->setAccDelay(thisLeg->getAccDelay() + delay);
		//nextLeg->setAccDual(thisLeg->getAccDual() + nextLeg->getDual());
		//nextLeg->setDelay(delay);

	}

	// cout << "thisLeg node Cost " << thisLeg->getNodeCost() << endl;
	// cout << "nextLeg node Cost " << nextLeg->getNodeCost() << endl;
}

vector<Lof* > Model::solveColGen()
{
	vector<Lof* > lofListSoln;

	_initColumns = findInitColumns();

	cout<<"###### initial Lofs have been generated ######"<<endl;

	populateByColumn(_initColumns);

	cout<<" ********************* LP SOLUTION 0 *********************"<<endl;
	solve();
	cout<<" ********************* END LP SOLUTION 0 *********************"<<endl<<endl;

	int count = 1;

	vector<Lof* > betterColumns;
	betterColumns = findNewColumns();

	while (!betterColumns.empty())
	{
		cout<<" ********************* LP SOLUTION "<< count << " *********************"<<endl<<endl; 

		addColumns(betterColumns);
		_initColumns.insert(_initColumns.end(),betterColumns.begin(),betterColumns.end());

		/*
		cout << "############### MEMORY TEST ###############" << endl;
		cout << "_initColumns.size() is " << _initColumns.size() << endl;

		if (_initColumns.size() > 120000)
		{
			cout << "############### MEMORY TEST ENDS ###############" << endl;
			exit(0);
		}
		*/

		solve();
		cout<<" ********************* END LP SOLUTION "<< count << " *********************"<<endl<<endl;

		count++;

		//if (count > 2) exit(0); //*DEBUG dual by CPLEX

		betterColumns = findNewColumns();
	}

	lofListSoln = solveIP();

	return lofListSoln;
}

vector<Lof* > Model::solveIP()
{
	cout<<" ********************* FINAL IP SOLUTION *********************"<<endl;

	_model.add(IloConversion(_env, _lofVar, ILOBOOL));
	_model.add(IloConversion(_env, _legVar, ILOBOOL));
	//* _model.add(IloConversion(_env, _grdArcVar, ILOINT));
	//* _model.add(IloConversion(_env, _terminalVar, ILOINT));

	//_solver = IloCplex(_model);

	_solver.exportModel("recovery.lp");

	_solver.solve();

	cout << endl;
	cout << "Number of leg variables is: " << _legVar.getSize() << endl;
	cout << "Number of lof variables is: " << _lofVar.getSize() << endl;
	cout << "Number of selection constraints is: " << _selectRng.getSize() << endl;
	cout << "Number of cover constraints is: " << _coverRng.getSize() << endl;
	cout << endl;

	_solver.out() << "Solution status: " << _solver.getStatus() << endl;
	_solver.out() << "Optimal value: " << _solver.getObjValue() << endl;

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
		for (int i = 0; i < lofVarSoln.getSize(); i++)    /// 为什么InitColums.size()有可能等于0呢？duplicated model IP
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

	_solver.setParam(IloCplex::RootAlg, IloCplex::Barrier); //* 设置求解LP的算法，用Barrier Scenario1能求到最优解
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
	//* 即模型里的y
	for (int j = 0; j < _legList.size(); j++)
	{
		itoa(_legList[j]->getId(), str, 10);
		string varName = string("y_") + string(str);

		double cancelCost = Util::w_cancelFlt; // cancel flight和maint的cost不一样
		if (_legList[j]->isMaint())
		{
			cancelCost = Util::w_cancelMtc;
		}

		_legVar.add(IloNumVar(_obj(cancelCost) + _coverRng[_legList[j]->getId()](1), 0, 1, ILOFLOAT, varName.c_str()));
	}

	//* add decision variable _lofVar
	//* 即模型里的x
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

vector<Lof* > Model::findInitColumns()
{
	vector<Lof* > initColumns;

	/* 其实没有initial Lof也能运行

	/*
	Lof* tempLof = new Lof();
	
	// 先手工设置一下初始的LoF, 方便debug
	// 一架飞机
	// 以test case (12)derived from(3), or (13) or (14) 为例，初始的Lof是 lg0 -> lg2 -> lg3 (显然lg0 -> lg1 -> lg4 -> lg3 应该更好)
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
	*/

	return initColumns;
}