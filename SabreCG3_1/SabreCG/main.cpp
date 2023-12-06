/*
#if defined(_MSC_VER)
#if !defined( _CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif
*/

#include "Util.h"
#include "Leg.h"
#include "Station.h"
#include "Aircraft.h"
#include "Model.h"
#include "Schedule.h"

int main() {
	/* test1 */
	/*
	Station st1("dummy_st1");
	Station st2("dummy_st2");
	Aircraft ac1("dummy_ac1", 1, 100, &st1, &st2);
	Leg lg1("lg1", &st1, &st2, 2, 90, &ac1);

	st1.print();
	st2.print();
	ac1.print();
	lg1.print();
	*/

	/* test2 */
	/*
	vector<Station *> stationList;
	vector<Aircraft *> aircraftList;
	vector<Leg *> legList;

	Station * station;
	station = new Station("dummy_st1");
	stationList.push_back(station);
	station = new Station("dummy_st2");
	stationList.push_back(station);

	stationList[0]->print();
	stationList[1]->print();

	Aircraft * aircraft;
	aircraft = new Aircraft("dummy_ac1", 1, 100, stationList[0], stationList[1]);
	aircraftList.push_back(aircraft);

	aircraftList[0]->print();

	Leg * leg;
	leg = new Leg("lg1", stationList[0], stationList[1], 2, 90, aircraftList[0]);
	legList.push_back(leg);

	legList[0]->print();

	Model model(stationList, aircraftList, legList);
	*/

	/* test DBL_MAX */
	/*
	double test = 10.76;
	cout << "DBL_MAX: " << DBL_MAX << endl;
	cout << "DBL_MAX: " << DBL_MAX + test << endl;
	test = DBL_MAX;
	cout << "test == DBL_MAX " << (test == DBL_MAX) << endl;
	*/

	/* small test case 1 */
	//* test case number (14)
	//* �ֹ����ú��࣬�������ɻ�
	// set up stations
	vector<Station *> stationList;
	Station * station;
	station = new Station("st0_A");
	stationList.push_back(station);

	station = new Station("st1_B");
	stationList.push_back(station);

	/*
	pair<time_t, time_t> closureTime; //* �ֹ�����airport st1_B�Ļ����ر�ʱ��
	closureTime.first = 20;
	closureTime.second = 25;
	station->pushCloseTime(closureTime);
	*/

	station = new Station("st2_C");
	stationList.push_back(station);

	/*
	pair<time_t, time_t> closureTime; //* �ֹ�����airport st2_C�Ļ����ر�ʱ��
	closureTime.first = 25;
	closureTime.second = 30;
	station->pushCloseTime(closureTime);
	*/

	station = new Station("st3_D");
	stationList.push_back(station);

	// set up aircraft
	vector<Aircraft *> aircraftList;
	Aircraft * aircraft;
	aircraft = new Aircraft("dummy_ac0", 0, 100, stationList[0], stationList[3]);
	//aircraft = new Aircraft("dummy_ac0", 0, 100, stationList[1], stationList[3]); // ���ԣ��ı�ac0 ��o-d pair
	//aircraft = new Aircraft("dummy_ac0", 0, 100, stationList[2], stationList[3]); // ���ԣ��ı�ac0 ��o-d pair
	aircraftList.push_back(aircraft);

	aircraft = new Aircraft("dummy_ac1", 0, 100, stationList[1], stationList[2]); // ����
	aircraftList.push_back(aircraft);

	// set up legs
	vector<Leg *> legList;
	Leg * leg;
	leg = new Leg("lg0", stationList[0], stationList[1], 0, 10, aircraftList[0]);
	legList.push_back(leg);
	leg->setDual(0);

	leg = new Leg("lg1", stationList[1], stationList[1], 10, 20, aircraftList[0]);
	legList.push_back(leg);
	leg->setDual(0);

	leg = new Leg("lg2", stationList[1], stationList[2], 19, 29, aircraftList[1]);
	//leg = new Leg("lg2", stationList[1], stationList[2], 19, 29, aircraftList[0]); // ����master problem, һ�ܷɻ�
	legList.push_back(leg);
	leg->setDual(0);

	leg = new Leg("lg3", stationList[2], stationList[3], 35, 45, aircraftList[0]);
	legList.push_back(leg);
	leg->setDual(0);

	leg = new Leg("lg4", stationList[1], stationList[2], 21, 25, aircraftList[0]);
	legList.push_back(leg);
	leg->setDual(0);

	// set up the connection among legs
	/*
	legList[0]->pushNextLeg(legList[1]);
	legList[1]->pushPrevLeg(legList[0]);

	legList[0]->pushNextLeg(legList[2]);
	legList[2]->pushPrevLeg(legList[0]);

	legList[1]->pushNextLeg(legList[2]);
	legList[2]->pushPrevLeg(legList[1]);

	legList[2]->pushNextLeg(legList[3]);
	legList[3]->pushPrevLeg(legList[2]);

	legList[1]->pushNextLeg(legList[4]);
	legList[4]->pushPrevLeg(legList[1]);
	
	legList[4]->pushNextLeg(legList[3]);
	legList[3]->pushPrevLeg(legList[4]);
	*/

	// set up topological order
	/*
	vector <Leg *> topOrderList;
	topOrderList.push_back(legList[0]);
	topOrderList.push_back(legList[1]);
	topOrderList.push_back(legList[2]);
	topOrderList.push_back(legList[4]);
	topOrderList.push_back(legList[3]);
	*/

	//cout << topOrderList.size() << endl;
	// ��leg��constructor���Ѿ�������dep station, arr station, maint station
	/*
	// setup dep legs of stations //
	stationList[0]->pushDepLeg(legList[0]);
	stationList[1]->pushMaint(legList[1]);
	stationList[1]->pushDepLeg(legList[2]);
	stationList[1]->pushDepLeg(legList[4]);
	stationList[2]->pushDepLeg(legList[3]);

	// setup arr legs of stations //
	stationList[1]->pushArrLeg(legList[0]);
	stationList[1]->pushArrLeg(legList[1]);
	stationList[2]->pushArrLeg(legList[2]);
	stationList[3]->pushArrLeg(legList[3]);
	stationList[2]->pushArrLeg(legList[4]);
	*/

	/*
	for (int i = 0; i < topOrderList.size(); i++) {
		topOrderList[i]->print();
	}
	*/

	/*
	legList[0]->setTopOrderNext(legList[1]);
	legList[1]->setTopOrderPrev(legList[0]);

	legList[1]->setTopOrderNext(legList[2]);
	legList[2]->setTopOrderPrev(legList[1]);

	legList[2]->setTopOrderNext(legList[3]);
	legList[3]->setTopOrderPrev(legList[2]);
	*/

	// ����aircraft��pushPlanLeg(), sortScheLegByDepTime();
	/*
	vector<Leg* > planLegList = aircraftList[0]->getPlanLegList();
	cout << "plan leg for aircraft before sort:" << endl;
	for (int i = 0; i < planLegList.size(); i++)
	{
		planLegList[i]->print();
	}
	cout << endl;

	aircraftList[0]->sortScheLegByDepTime();

	cout << "plan leg for aircraft after sort:" << endl;
	planLegList = aircraftList[0]->getPlanLegList();
	for (int i = 0; i < planLegList.size(); i++)
	{
		planLegList[i]->print();
	}
	cout << endl;

	cout << "isPlanLegFeasible: " << aircraftList[0]->isPlanLegFeasible() << endl;
	*/

	// ����Schedule class
	
	Schedule schedule(stationList, aircraftList, legList);
	//schedule.setAdjascentLeg(); //* schedule��constructor���Ѿ�������setAdjascentLeg();
	schedule.computeTopOrder();
	

	// create Model object
	// solve the test problem
	// return the selected lofs
	Model model(stationList, aircraftList, legList, schedule.getTopOrderList());
	vector<Lof* > resultLof = model.solveColGen();
	for (int i = 0; i < resultLof.size(); i++)
	{
		resultLof[i]->print();
	}

	// ����findInitColumns
	/*
	vector<Lof* > resultLof = model.findInitColumns();
	for (int i = 0; i < resultLof.size(); i++)
	{
		resultLof[i]->print();
	}
	*/

	// ����findInitOneColumn
	/*
	Lof* testLof = model.findInitOneColumn(aircraftList[0]);
	testLof->print();
	*/

	// ����computeTopOrder
	/*
	vector<Leg* > topOrderList = schedule.getTopOrderList();
	cout << "size of computed topOrderList is: " << topOrderList.size() << endl;
	cout << "############## PRINTING TOPOLOGICAL ORDER LEGS ###############" << endl;
	for (int i = 0; i < topOrderList.size(); i++)
	{
		topOrderList[i]->print();
	}
	cout << "############## END PRINTING TOPOLOGICAL ORDER LEGS ###############" << endl;
	*/

	/*
	cout << "****** information of all legs ******" << endl;
	for (int i = 0; i < topOrderList.size(); i++) // ��ӡ���
	{
		topOrderList[i]->print();
		cout << endl;
	}
	*/

	//lof->print();

	// ����solveColGen()
	//model.solveColGen();

	// ����model:: findInitColumns()
	/*
	vector<Lof* > testLofVector = model.findInitColumns();
	//testLofVector[0]->print();
	*/

	// ����populateByColumn
	//model.populateByColumn(testLofVector);

	// ����model:: findNewOneColumn;
	/*
	Lof* testLof = model.findNewOneColumn(aircraftList[0]);

	testLof->computeLofCost();
	cout << "computed cost by lof: " << testLof->getCost() << endl;

	testLof->computeReducedCost();
	cout << "computed reduced cost by lof: " << testLof->getReducedCost() << endl;

	testLof->print();
	*/

	// system("pause");
}