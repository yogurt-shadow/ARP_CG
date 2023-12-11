#ifndef UTIL_H
#define UTIL_H

#include<iostream>
#include<stdio.h>
#include <algorithm>
#include<string>
#include<ctime>
#include<vector>
#include<fstream>
#include<ilcplex/ilocplex.h>
#include<ctime>
#include<map>
#include<process.h>
#include<stack>
#include<list>

ILOSTLBEGIN;
using namespace std;

#define MAXTURNTIME (5*60*60)           // 5 hours 
#define DATECUTTIME (3*60*60)           // 3 o'clock mid night 
#define DATEDURATION (24 * 60 * 60)     // one day time
#define MINLEG 1
#define MAXLEG 7 
//*#define TIMEDIFF (8*60*60)              // China Time difference
#define TIMEDIFF 0
#define INITLOFNUM 10000                // Number of Lofs Randomly selected
#define SELECTNUM 100                  // Number of Lofs put into Model after each generation
#define SEED 1

class Util
{
public :
	static int turnTime;
	static int maxDelayTime;
	static int w_cancelMtc;
	static int w_cancelFlt;
	static int w_violatedBalance;
	static int w_violatedPosition;
	static int w_fltDelay;
	static int w_fltSwap;
	static int maxRunTime;

	static int newamount;		// һ��subproblemÿ�����lof�ĸ���

	static void print() {
		cout << "print parameters" << endl;
		cout << "turnTime = " << turnTime << endl;
		cout << "maxDelayTime = " << maxDelayTime << endl;
		cout << "w_cancelMtc = " << w_cancelMtc << endl;
		cout << "w_cancelFlt = " << w_cancelFlt << endl;
		cout << "w_violatedBalance = " << w_violatedBalance << endl;
		cout << "w_violatedPosition = " << w_violatedPosition << endl;
		cout << "w_fltDelay = " << w_fltDelay << endl;
		cout << "w_fltSwap = " << w_fltSwap << endl;
		cout << "maxRunTime = " << maxRunTime << endl;
		cout << "newamount = " << newamount << endl;
	
	}
};

/*
int getDate(time_t t)
{
	return (t - DATECUTTIME)/DATEDURATION;        /// �������Ǵ�0��ʼ�����
}
*/

#endif