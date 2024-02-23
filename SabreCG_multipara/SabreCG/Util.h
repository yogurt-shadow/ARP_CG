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
#define TIMEDIFF (8*60*60)              // China Time difference
#define INITLOFNUM 10000                // Number of Lofs Randomly selected
#define SELECTNUM 100                  // Number of Lofs put into Model after each generation
#define SEED 1
//#define NEWAMOUNT 1 //max amount of new columns 
//#define CPLEXERROR 0.001
#define THREADSIZE 8					// ！！！改成thread最大可能的数目，用来定义一些相关数组的大小

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

	static int newamount;
	static int threadSize;
};

/*
int getDate(time_t t)
{
	return (t - DATECUTTIME)/DATEDURATION;        /// 天数不是从0开始计算的
}
*/

#endif