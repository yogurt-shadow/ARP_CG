//* 用来测试multi label 20170108
#include "Util.h"
#include "Leg.h"
#include "Station.h"
#include "Aircraft.h"
#include "Model.h"

int main()
{
	/* small test case 1 */
	//* test case number (14)
	//* 手工设置航班，机场，飞机
	// set up stations
	vector<Station *> stationList;
	Station * station;
	station = new Station("st0_A");
	stationList.push_back(station);

	station = new Station("st1_B");
	stationList.push_back(station);

	/*
	pair<time_t, time_t> closureTime; //* 手工设置airport st1_B的机场关闭时间
	closureTime.first = 15;
	closureTime.second = 25;
	station->pushCloseTime(closureTime);
	*/

	station = new Station("st2_C");
	stationList.push_back(station);

	/*
	pair<time_t, time_t> closureTime; //* 手工设置airport st2_C的机场关闭时间
	closureTime.first = 40;
	closureTime.second = 45;
	station->pushCloseTime(closureTime);
	*/

	// set up aircraft
	vector<Aircraft *> aircraftList;
	Aircraft * aircraft;
	aircraft = new Aircraft("dummy_ac0", 0, 100, stationList[0], stationList[2]);
	aircraftList.push_back(aircraft);

	aircraft = new Aircraft("dummy_ac1", 0, 100, stationList[0], stationList[2]);
	aircraftList.push_back(aircraft);

	// set up legs
	vector<Leg *> legList;
	Leg * leg;
	leg = new Leg("lg0", stationList[0], stationList[1], 0, 10, aircraftList[0]);
	//leg = new Leg("lg0", stationList[0], stationList[0], 0, 10, aircraftList[0]);
	legList.push_back(leg);

	leg = new Leg("lg1", stationList[1], stationList[2], 10, 20, aircraftList[0]);
	//leg = new Leg("lg1", stationList[0], stationList[0], 21, 31, aircraftList[0]);	// maintenance
	legList.push_back(leg);

	// set up the connection among legs
	legList[0]->pushNextLeg(legList[1]);

	//legList[0]->print();

	// 手工设置topological order
	vector <Leg *> topOrderList;
	topOrderList.push_back(legList[0]);
	topOrderList.push_back(legList[1]);

	Model model(stationList, aircraftList, legList, topOrderList);
	model.findNewOneColumn(aircraftList[0]);

	// 测试edgeProcessFlt
	/*
	model.edgeProcessFlt(legList[0], aircraftList[0]);
	legList[0]->print();
	*/

	// 测试edgeProcessMaint
	/*
	model.edgeProcessMaint(legList[0], aircraftList[1]);
	legList[0]->print();
	*/

	// 测试insertSubNode
	/*
	SubNode* subNode = new SubNode(legList[1], NULL, 100, 50);
	legList[1]->pushSubNode(subNode);

	//subNode = new SubNode(_legList[0], NULL, 14, 11);
	subNode = new SubNode(legList[1], NULL, 90, 70);
	legList[1]->pushSubNode(subNode);

	//subNode = new SubNode(_legList[0], NULL, 13, 12);
	subNode = new SubNode(legList[1], NULL, 80, 80);
	legList[1]->pushSubNode(subNode);

	cout << "before " << endl;
	legList[1]->print();

	subNode = new SubNode(legList[1], NULL, 50, 60);
	legList[1]->insertSubNode(subNode);

	cout << "after " << endl;
	legList[1]->print();
	*/


	// 测试lessSubNodePointer
	/*
	SubNode* subNode1 = new SubNode(legList[0], NULL, 13, 11);
	SubNode* subNode2 = new SubNode(legList[0], NULL, 14, 10);
	cout << "lessSubNodePointer " << SubNode::lessSubNodePointer(subNode2, subNode1) << endl;
	*/
	
}