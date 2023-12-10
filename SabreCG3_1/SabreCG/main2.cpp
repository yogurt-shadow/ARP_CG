#if defined( _MSC_VER ) 
#if !defined( _CRT_SECURE_NO_WARNINGS ) 
#define _CRT_SECURE_NO_WARNINGS		// This test file is not intended to be secure. 
#endif 
#endif 


#include"Util.h"
#include "tinyxml2.h" 
#include <cstdlib> 
#include <cstring> 
#include <ctime> 
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>


#include"Aircraft.h"
#include"Leg.h"
#include"Station.h"
#include"Schedule.h"
#include"Model.h"


#if defined( _MSC_VER ) 
#include <direct.h>		// _mkdir 
#include <crtdbg.h> 
#define WIN32_LEAN_AND_MEAN 
#include <windows.h> 
_CrtMemState startMemState; 
_CrtMemState endMemState; 
#elif defined(MINGW32) || defined(__MINGW32__) 
#include <io.h>  // mkdir 
#else 
#include <sys/stat.h>	// mkdir 
#endif 


using namespace tinyxml2; 
using namespace std;

struct flightType
{
	std::string id;	/* required element of type xsdtring */
	int departureTime;	/* required element of type xsd:int */
	int arrivalTime;	/* required element of type xsd:int */
	std::string departureAirport;	/* required element of type xsdtring */
	std::string arrivalAirport;	/* required element of type xsdtring */
	std::string tailNumber;	/* required element of type xsdtring */
};
struct aircraftType
{
	std::string tailNumber;	/* required element of type xsdtring */
	int startAvailableTime;	/* required element of type xsd:int */
	int endAvailableTime;	/* required element of type xsd:int */
	std::string startAvailableAirport;	/* required element of type xsdtring */
	std::string endAvailableAirport;	/* required element of type xsdtring */
};
struct mtcType
{
	std::string id;	/* required element of type xsdtring */
	int startTime;	/* required element of type xsd:int */
	int endTime;	/* required element of type xsd:int */
	std::string airport;	/* required element of type xsdtring */
	std::string tailNumber;	/* required element of type xsdtring */
};
struct airportClosureType
{
	std::string code;	/* required element of type xsdtring */
	int startTime;	/* required element of type xsd:int */
	int endTime;	/* required element of type xsd:int */
};
struct paraSet
{
	int turnTime;
	int maxDelayTime;
	int w_cancelMtc;
	int w_cancelFlt;
	int w_violatedBalance;
	int w_violatedPosition;
	int w_fltDelay;
	int w_fltSwap;
	int maxRunTime;
};

// Data container
vector<flightType> flights;
vector<mtcType> mtcs;
vector<aircraftType> aircrafts;
vector<airportClosureType> airportClosures;
paraSet parameters;

bool importAircarfts(const std::string& fullFileName, vector<aircraftType>& input_aircrafts)
{ 
	std::string line, content;
	std::ifstream inputFile(fullFileName.c_str());
	if (!inputFile)
	{
		std::cout << "Error while reading aircrafts!";
		return false;
	}
	while (std::getline(inputFile, line))
	{
		content += line;
	}

	XMLDocument doc; 
	doc.Parse( content.c_str() ); 
	XMLElement* eleChild1 =doc.FirstChildElement()->FirstChildElement( "ns1:aircraft" );

	while (eleChild1)
	{		
		aircraftType acObj;
		acObj.tailNumber = eleChild1->FirstChildElement("ns1:tailNumber")->GetText();

		unsigned int st = 0, et = 0;
		eleChild1->FirstChildElement( "ns1:startAvailableTime" )->QueryUnsignedText( &st ); 
		eleChild1->FirstChildElement("ns1:endAvailableTime")->QueryUnsignedText(&et);
		acObj.startAvailableTime = st;
		acObj.endAvailableTime = et;
		acObj.startAvailableAirport = eleChild1->FirstChildElement( "ns1:startAvailableAirport" )->GetText(); 
		acObj.endAvailableAirport = eleChild1->FirstChildElement( "ns1:endAvailableAirport" )->GetText(); 

		input_aircrafts.push_back(acObj);
		eleChild1 = eleChild1->NextSiblingElement();
	}

	return true;

} 

bool importAirportClosures(const std::string& fullFileName, vector<airportClosureType>& input_airportClosures)
{
	std::string line, content;
	std::ifstream inputFile(fullFileName.c_str());
	if (!inputFile)
	{
		std::cout << "Error while reading airport closures!";
		return false;
	}
	while (std::getline(inputFile, line))
	{
		content += line;
	}

	XMLDocument doc;
	doc.Parse(content.c_str());
	XMLElement* eleChild1 = doc.FirstChildElement()->FirstChildElement("ns2:flow");

	while (eleChild1)
	{
		airportClosureType flowObj;
		unsigned int st = 0, et = 0;
		flowObj.code = eleChild1->FirstChildElement("ns2:code")->GetText();
		eleChild1->FirstChildElement("ns2:startTime")->QueryUnsignedText(&st);
		eleChild1->FirstChildElement("ns2:endTime")->QueryUnsignedText(&et);
		flowObj.startTime = st;
		flowObj.endTime = et;

		input_airportClosures.push_back(flowObj);
		eleChild1 = eleChild1->NextSiblingElement();
	}

	return true;

}

bool importSchedules(const std::string& fullFileName, vector<flightType>& input_flights, vector<mtcType>& input_mtcs)
{
	std::string line, content;
	std::ifstream inputFile(fullFileName.c_str());
	if (!inputFile)
	{
		std::cout << "Error while reading schedules!";
		return false;
	}
	while (std::getline(inputFile, line))
	{
		content += line;
	}

	XMLDocument doc;
	doc.Parse(content.c_str());
	XMLElement* xml_fltList = doc.FirstChildElement()->FirstChildElement("ns3:flightInfoList");
	XMLElement* xml_flt = xml_fltList->FirstChildElement("ns3:flightInfo");
	XMLElement* xml_mtcList = doc.FirstChildElement()->FirstChildElement("ns3:mtcInfoList");
	XMLElement* xml_mtc = NULL;
	if (xml_mtcList)
	{
		xml_mtc = xml_mtcList->FirstChildElement("ns3:mtcInfo");
	}

	while (xml_flt)
	{
		flightType flightObj;
		unsigned int st = 0, et = 0;
		flightObj.id = xml_flt->FirstChildElement("ns3:id")->GetText();
		xml_flt->FirstChildElement("ns3:departureTime")->QueryUnsignedText(&st);
		xml_flt->FirstChildElement("ns3:arrivalTime")->QueryUnsignedText(&et);
		flightObj.departureTime = st;
		flightObj.arrivalTime = et;
		flightObj.departureAirport = xml_flt->FirstChildElement("ns3:departureAirport")->GetText();
		flightObj.arrivalAirport = xml_flt->FirstChildElement("ns3:arrivalAirport")->GetText();
		flightObj.tailNumber = xml_flt->FirstChildElement("ns3:tailNumber")->GetText();

		input_flights.push_back(flightObj);
		xml_flt = xml_flt->NextSiblingElement();
	}

	while (xml_mtc)
	{
		mtcType mtcObj;
		unsigned int st = 0, et = 0;
		mtcObj.id = xml_mtc->FirstChildElement("ns3:id")->GetText();
		xml_mtc->FirstChildElement("ns3:startTime")->QueryUnsignedText(&st);
		xml_mtc->FirstChildElement("ns3:endTime")->QueryUnsignedText(&et);
		mtcObj.startTime = st;
		mtcObj.endTime = et;
		mtcObj.airport = xml_mtc->FirstChildElement("ns3:airport")->GetText();
		mtcObj.tailNumber = xml_mtc->FirstChildElement("ns3:tailNumber")->GetText();

		input_mtcs.push_back(mtcObj);
		xml_mtc = xml_mtc->NextSiblingElement();
	}

	return true;

}

bool importParameters(const std::string& fullFileName, paraSet& input_parameters)
{
	std::string line, content;
	std::ifstream inputFile(fullFileName.c_str());
	if (!inputFile)
	{
		std::cout << "Error while reading aircrafts!";
		return false;
	}
	while (std::getline(inputFile, line))
	{
		content += line;
	}

	unsigned int turnTime = 0, maxDelayTime = 0, w_cancelMtc = 0, w_cancelFlt = 0,
		w_violatedBalance = 0, w_violatedPosition = 0, w_fltDelay = 0, w_fltSwap = 0, maxRunTime = 0;


	XMLDocument doc;
	doc.Parse(content.c_str());
	XMLElement* eleChild1 = doc.FirstChildElement();


	eleChild1->FirstChildElement("turnTime")->QueryUnsignedText(&turnTime);
	eleChild1->FirstChildElement("maxDelayTime")->QueryUnsignedText(&maxDelayTime);
	eleChild1->FirstChildElement("weightCancelMaintenance")->QueryUnsignedText(&w_cancelMtc);
	eleChild1->FirstChildElement("weightCancelFlight")->QueryUnsignedText(&w_cancelFlt);
	eleChild1->FirstChildElement("weightViolateBalance")->QueryUnsignedText(&w_violatedBalance);
	eleChild1->FirstChildElement("weightViolatePositioning")->QueryUnsignedText(&w_violatedPosition);
	eleChild1->FirstChildElement("weightFlightDelay")->QueryUnsignedText(&w_fltDelay);
	eleChild1->FirstChildElement("weightFlightSwap")->QueryUnsignedText(&w_fltSwap);
	eleChild1->FirstChildElement("maxRunTime")->QueryUnsignedText(&maxRunTime);

	input_parameters.turnTime = turnTime;
	input_parameters.maxDelayTime = maxDelayTime;
	input_parameters.w_cancelMtc = w_cancelMtc;
	input_parameters.w_cancelFlt = w_cancelFlt;
	input_parameters.w_violatedBalance = w_violatedBalance;
	input_parameters.w_violatedPosition = w_violatedPosition;
	input_parameters.w_fltDelay = w_fltDelay;
	input_parameters.w_fltSwap = w_fltSwap;
	input_parameters.maxRunTime = maxRunTime;

	return true;

}

bool exportSolution(const std::string& output_path,vector<Leg *> _LegList)
{
	/* update flights and maints information */

	try{
		XMLDocument doc;
		XMLElement* rootElement = doc.NewElement("exportAircrafts");

		// Flight Info
		XMLElement* xml_fltList = doc.NewElement("ns3:flightInfoList");
		for (vector<Leg *>::const_iterator i_flt = _LegList.begin(); i_flt != _LegList.end(); ++i_flt)
		{
			if (!(*i_flt)->isMaint())
			{
				XMLElement* xml_fltType = doc.NewElement("ns3:flightInfo");
				XMLText* xml_value;

				XMLElement* xml_flt_id = doc.NewElement("ns3:id");
				xml_value = doc.NewText((*i_flt)->getFlightNum().c_str());
				xml_flt_id->InsertEndChild(xml_value);
				xml_fltType->InsertEndChild(xml_flt_id);

				XMLElement* xml_flt_depTime = doc.NewElement("ns3:departureTime");
				xml_value = doc.NewText(std::to_string((*i_flt)->getDepTime()).c_str());
				xml_flt_depTime->InsertEndChild(xml_value);
				xml_fltType->InsertEndChild(xml_flt_depTime);

				XMLElement* xml_flt_arrTime = doc.NewElement("ns3:arrivalTime");
				xml_value = doc.NewText(std::to_string((*i_flt)->getArrTime()).c_str());
				xml_flt_arrTime->InsertEndChild(xml_value);
				xml_fltType->InsertEndChild(xml_flt_arrTime);

				XMLElement* xml_flt_depArp = doc.NewElement("ns3:departureAirport");
				xml_value = doc.NewText((*i_flt)->getDepStation()->getName().c_str());
				xml_flt_depArp->InsertEndChild(xml_value);
				xml_fltType->InsertEndChild(xml_flt_depArp);

				XMLElement* xml_flt_arrArp = doc.NewElement("ns3:arrivalAirport");
				xml_value = doc.NewText((*i_flt)->getArrStation()->getName().c_str());
				xml_flt_arrArp->InsertEndChild(xml_value);
				xml_fltType->InsertEndChild(xml_flt_arrArp);

				XMLElement* xml_flt_tailNum = doc.NewElement("ns3:tailNumber");
				xml_value = doc.NewText((*i_flt)->getAircraft()->getTail().c_str());
				xml_flt_tailNum->InsertEndChild(xml_value);
				xml_fltType->InsertEndChild(xml_flt_tailNum);

				XMLElement* xml_flt_status = doc.NewElement("ns3:status");
				if ((*i_flt)->getAssigned() == true)
				{
					xml_value = doc.NewText("Assigned"); // customize here
				}
				else
				{
					xml_value = doc.NewText("Cancelled"); // customize here
				}

				xml_flt_status->InsertEndChild(xml_value);
				xml_fltType->InsertEndChild(xml_flt_status);

				xml_fltList->InsertEndChild(xml_fltType);
			}


		}
		// Maintenance Info
		XMLElement* xml_mtcList = doc.NewElement("ns3:mtcInfoList");
		for (vector<Leg *>::const_iterator i_mtc = _LegList.begin(); i_mtc != _LegList.end(); ++i_mtc)
		{
			if ( (*i_mtc)->isMaint())
			{
				XMLElement* xml_mtcType = doc.NewElement("ns3:mtcInfo");
				XMLText* xml_value;

				XMLElement* xml_mtc_id = doc.NewElement("ns3:id");
				xml_value = doc.NewText((*i_mtc)->getFlightNum().c_str());
				xml_mtc_id->InsertEndChild(xml_value);
				xml_mtcType->InsertEndChild(xml_mtc_id);

				XMLElement* xml_mtc_startTime = doc.NewElement("ns3:startTime");
				xml_value = doc.NewText(std::to_string((*i_mtc)->getDepTime()).c_str());
				xml_mtc_startTime->InsertEndChild(xml_value);
				xml_mtcType->InsertEndChild(xml_mtc_startTime);

				XMLElement* xml_mtc_endTime = doc.NewElement("ns3:endTime");
				xml_value = doc.NewText(std::to_string((*i_mtc)->getArrTime()).c_str());
				xml_mtc_endTime->InsertEndChild(xml_value);
				xml_mtcType->InsertEndChild(xml_mtc_endTime);

				XMLElement* xml_mtc_arp = doc.NewElement("ns3:airport");
				xml_value = doc.NewText((*i_mtc)->getDepStation()->getName().c_str());
				xml_mtc_arp->InsertEndChild(xml_value);
				xml_mtcType->InsertEndChild(xml_mtc_arp);

				XMLElement* xml_mtc_tailNum = doc.NewElement("ns3:tailNumber");
				xml_value = doc.NewText((*i_mtc)->getAircraft()->getTail().c_str());
				xml_mtc_tailNum->InsertEndChild(xml_value);
				xml_mtcType->InsertEndChild(xml_mtc_tailNum);

				XMLElement* xml_mtc_status = doc.NewElement("ns3:status");
				if ((*i_mtc)->getAssigned() == true)
				{
					xml_value = doc.NewText("Assigned"); // customize here
				}
				else
				{
					xml_value = doc.NewText("Cancelled"); // customize here
				}


				xml_mtc_status->InsertEndChild(xml_value);
				xml_mtcType->InsertEndChild(xml_mtc_status);

				xml_mtcList->InsertEndChild(xml_mtcType);
			}


		}
		rootElement->InsertEndChild(xml_fltList);
		rootElement->InsertEndChild(xml_mtcList);
		doc.InsertEndChild(rootElement);
		if (!doc.SaveFile((output_path + "Output.xml").c_str()))
			return true;
	}
	catch (...)
	{
		return false;
	}
	return true;
}

void readConfiguarationFile(std::string& input_dir, std::string& output_dir)
{
	std::string configFile = "./Config.txt";
	std::ifstream inputFile(configFile.c_str());
	if (!inputFile)
	{
		return;
	}
	std::string fieldName[3];
	int i = 0;
	std::cout << "\nReading the configuration file" << std::endl;
	while (!inputFile.eof())
	{
		inputFile >> fieldName[i];
		if (fieldName[i] == "END")
			break;
		else if (fieldName[i] == "INPUT_DIRECTORY")
		{
			inputFile >> input_dir;
			input_dir += "\\";

			std::cout << fieldName[i] << "     " << input_dir << std::endl;
			i++;
			continue;
		}

		if (fieldName[i] == "OUTPUT_DIRECTORY")
		{
			inputFile >> output_dir;
			output_dir += "\\";

			std::cout << fieldName[i] << "     " << output_dir << std::endl;
			i++;
			continue;
		}

		i++;
		if (i == 2)
			break;
	}
	std::cout << "" << std::endl;
	inputFile.close();
	return;
}

bool processArguments(int argc, char** argv, std::string& input_dir, std::string& output_dir)
{
	bool isCheck = false;
	for (int i = 1; i < argc; i++)
	{
		if (i == 1)
		{
			input_dir += (argv[i]);
			input_dir += "\\";
		}
		else if (i == 2)
		{
			output_dir += (argv[i]);
			isCheck = true;
			output_dir += "\\";
		}
	}
	cout <<"input_dir is " << input_dir << endl;
	cout <<"output_dir is " << output_dir << endl;
	return isCheck;
}

vector<Leg *> updaInfo(vector<Lof *> _LofListSoln,vector<Leg *> _InitLegList) ///?????????????????????????????????
{
	vector<Leg *> legList;
	cout << "test for upda" << endl;
	/* dealing with assigned flights */
	for (int i = 0; i < _LofListSoln.size(); i++)
	{
		vector<OperLeg *> _OperLegList;
		_OperLegList = _LofListSoln[i]->getLegList();

		for (int j = 0; j < _OperLegList.size(); j++)
		{
			_OperLegList[j]->print();
			Station * depStation = _OperLegList[j]->getLeg()->getDepStation();
			Station * arrStation = _OperLegList[j]->getLeg()->getArrStation();

			time_t depTime = _OperLegList[j]->getPrintDepTime();
			time_t arrTime = _OperLegList[j]->getPrintArrTime();

			string flightNum = _OperLegList[j]->getLeg()->getFlightNum();
			Aircraft * aircraft = _LofListSoln[i]->getAircraft();

			Leg * leg = new Leg(flightNum, depStation, arrStation, depTime, arrTime, aircraft);
			leg->setAssigned(true);

			if (_OperLegList[j]->getLeg()->isMaint())
			{
				leg->setIsMaint(true);
			}
			else
			{
				leg->setIsMaint(false);
			}

			legList.push_back(leg);
		}
	}

	/* dealing with cancelled flights */
	for (int i = 0; i < _InitLegList.size(); i++)
	{
		bool found = false;
		for (int j = 0; j < legList.size(); j++)
		{
			if (_InitLegList[i]->getFlightNum() == legList[j]->getFlightNum())
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			_InitLegList[i]->setAssigned(false);
			legList.push_back(_InitLegList[i]);
		}
	}

	return legList;
}

int main( int argc, char * argv[] ) 
{ 

	cout << "program start..." << endl;
#if defined( _MSC_VER ) && defined( DEBUG ) 
	_CrtMemCheckpoint( &startMemState ); 
	// Enable MS Visual C++ debug heap memory leaks dump on exit 
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF); 
#endif 


#if defined(_MSC_VER) || defined(MINGW32) || defined(__MINGW32__) 
#if defined __MINGW64_VERSION_MAJOR && defined __MINGW64_VERSION_MINOR 
	//MINGW64: both 32 and 64-bit 
	mkdir( "resources/out/" ); 
#else 
	_mkdir( "resources/out/" ); 
#endif 
#else 
	mkdir( "resources/out/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 
#endif 


	{ 
		TIXMLASSERT( true ); 
	} 

	std::string input_path, output_path;
	if (!processArguments(argc, argv, input_path, output_path)) {
		cout << "read config" << endl;
		readConfiguarationFile(input_path, output_path);
	}
	else {
		cout << "skip config" << endl;
	}
	if (input_path.empty() || output_path.empty())
	{
		std::cout << "Either input directory or output directory is not specified." << std::endl;
		return 0;
	}
	if(!importAircarfts(input_path + "Aircraft.xml", aircrafts))
		return 0;
	if (!importAirportClosures(input_path + "AirportClosure.xml", airportClosures))
		return 0;
	if (!importSchedules(input_path + "Schedule.xml", flights, mtcs))
		return 0;
	if (!importParameters(input_path + "Parameters.xml", parameters))
		return 0;
	//Initialize the parameters 
	Util::maxDelayTime = parameters.maxDelayTime;
	Util::maxRunTime = parameters.maxRunTime;
	Util::turnTime = parameters.turnTime;
	Util::w_cancelFlt = parameters.w_cancelFlt;
	Util::w_cancelMtc = parameters.w_cancelMtc;
	Util::w_fltDelay = parameters.w_fltDelay;
	Util::w_fltSwap = parameters.w_fltSwap;
	Util::w_violatedBalance = parameters.w_violatedBalance;
	Util::w_violatedPosition = parameters.w_violatedPosition;


	vector<Station *> stationList;
	vector<Aircraft *> aircraftList;

	/*Initialize the aircraft information and station Information*/
	for(int i = 0; i < aircrafts.size(); i++)
	{
		string tail = aircrafts[i].tailNumber;
		time_t startTime = aircrafts[i].startAvailableTime;
		time_t endTime = aircrafts[i].endAvailableTime;

		string depName = aircrafts[i].startAvailableAirport;
		string arrName = aircrafts[i].endAvailableAirport;
		Station * depStation = NULL;
		Station * arrStation = NULL;

		for(int j = 0; j < stationList.size(); j++)  /// ����stationList�б����station pointer, ���depStation��û�м���stationList, ����֮; �õ�depStation, arrStation��ָ��
		{
			if (stationList[j]->getName() == depName)
			{
				depStation = stationList[j];
				break;
			}
		}
		if (depStation == NULL)
		{
			depStation = new Station(depName);
			stationList.push_back(depStation);

		}

		for(int j =0; j < stationList.size(); j++)
		{
			if (stationList[j]->getName() == arrName)
			{
				arrStation = stationList[j];
				break;
			}
		}
		if (arrStation == NULL)
		{
			arrStation = new Station(arrName);
			stationList.push_back(arrStation);
		}

		Aircraft * aircraft = new Aircraft(tail, startTime, endTime, depStation, arrStation);
		aircraftList.push_back(aircraft);
	}

	//	for(int i = 0; i < aircraftList.size(); i++)
	//	{
	//		aircraftList[i]->print();
	//	}



	/*Initialize the LegList, flights+maint*/
	vector<Leg *> legList;
	/*for ordinary flights*/
	for(int i = 0; i < flights.size(); i++)
	{
		string depName = flights[i].departureAirport;
		string arrName = flights[i].arrivalAirport;

		Station * depStation = NULL;
		Station * arrStation = NULL;

		time_t depTime = flights[i].departureTime;
		time_t arrTime = flights[i].arrivalTime;

		string flightNum = flights[i].id;

		string tail = flights[i].tailNumber;
		Aircraft * aircraft = NULL;

		/*set the departure airport*/
		for(int j = 0; j < stationList.size(); j++)
		{
			if (stationList[j]->getName() == depName)
			{
				depStation = stationList[j];
				break;
			}
		}

		if (depStation == NULL)
		{
			depStation = new Station(depName);
			stationList.push_back(depStation);
		}

		/*set the arrival airport*/
		for(int j =0; j < stationList.size(); j++)
		{
			if (stationList[j]->getName() == arrName)
			{
				arrStation = stationList[j];
				break;
			}
		}

		if (arrStation == NULL)
		{
			arrStation = new Station(arrName);
			stationList.push_back(arrStation);
		}

		/*set the scheduled aircraft of the Leg*/
		for(int j = 0; j < aircraftList.size(); j++)
		{
			if ( aircraftList[j]->getTail() == tail)
			{
				aircraft = aircraftList[j];
				break;
			}
		}

		if(aircraft == NULL)
		{
			cout <<"Cannot Find Aircraft "<< tail << endl;
			exit(0);
		}

		Leg * leg = new Leg(flightNum, depStation, arrStation, depTime, arrTime, aircraft);
		legList.push_back(leg);
	}

	for (int i = 0; i < aircraftList.size(); i++) /// ���û�����departure aircraft��Ϣ
	{
		Station * depStation = aircraftList[i]->getDepStation();
		depStation->pushDepAircraft(aircraftList[i]);
	}

	cout<<"********************* Number of Aircraft Departured *********************"<<endl;
	for (int i = 0; i < stationList.size(); i++)
	{
		cout<<"number of aircraft departured from station "<<stationList[i]->getName()<<" is "
			<<stationList[i]->getDepAircraftNumber()<<endl;
	}
	cout<<"********************* END PRINTING *********************"<<endl;

	for (int i = 0; i < stationList.size(); i++)
	{
		stationList[i]->setLegNum(legList.size()); // ��Ȼ����station��legNum����һ���ģ����ﲻ��legNum��Ϊstatic?
	}

	// for(int i = 0; i < legList.size(); i++)
	// {
	//	 legList[i]->print();
	// }

	/*Initialize maintList,only includes maintenance*/
	vector<Leg *> maintList;
	for(int i = 0; i < mtcs.size(); i++)
	{
		string stationName = mtcs[i].airport;

		Station * station = NULL;

		time_t startTime = mtcs[i].startTime;
		time_t endTime = mtcs[i].endTime;

		string maintId = mtcs[i].id;

		string tail = mtcs[i].tailNumber;

		for(int j = 0; j < stationList.size(); j++)
		{
			if (stationList[j]->getName() == stationName)
			{
				station = stationList[j];
				break;
			}
		}
		if (station == NULL)
		{
			station = new Station(stationName);
			stationList.push_back(station);
		}

		Aircraft * aircraft = NULL;
		for(int j = 0; j < aircraftList.size(); j++)
		{
			if ( aircraftList[j]->getTail() == tail)
			{
				aircraft = aircraftList[j];
				// should we break ?
			}
		}

		if(aircraft == NULL)
		{
			cout <<"Cannot Find Aircraft "<< tail << endl;
			exit(0);
		}

		Leg * maint = new Leg(maintId, station, station, startTime, endTime, aircraft);
		maintList.push_back(maint);      // only maint
		legList.push_back(maint);        // flights+maint
	}

	 //for(int i = 0; i < maintList.size(); i++)
	 //{
		// maintList[i]->print();
	 //}

	/* set the closure time of station */
	for(int i = 0; i < airportClosures.size(); i++)
	{
		string stationName = airportClosures[i].code;
		time_t startT = airportClosures[i].startTime;
		time_t endT = airportClosures[i].endTime;

		Station * station = NULL;
		for(int j =0; j < stationList.size(); j++)
		{
			if ( stationList[j]->getName() == stationName)
			{
				station = stationList[j];
				break;
			}
		}

		if (station == NULL)
		{
			cout <<"Error, Cannot Find Station " << stationName << endl;
			exit(0);
		}
		pair<time_t, time_t> closeTime;  /// ��һ��pair ����closeTime, ��ʼ�ͽ���ʱ��
		closeTime.first= startT;
		closeTime.second = endT;
		station->pushCloseTime(closeTime);
	}

	//	for(int i = 0; i < maintList.size(); i++)
	//	{
	//		maintList[i]->print();
	//	}


	/*Data Reading has done!*/
	std::cout << "Ready to go!" << std::endl;

	/*
	Schedule * schedule = new Schedule(stationList, aircraftList, legList);

	/// change some sequences

	//* generate feasible lofs by scheduledLegs 
	schedule->createLof();

	schedule->setOperLofList(); /// added Ȼ����û��ʲô��

	//* Init time node related to aircraft and station
	schedule->initAircraftNodes();

	//* delay cross day lofs
	schedule->delayLofsCrossDay();

	//* compute lof cost
	schedule->computeLofCost();

//	schedule->print();

	//	schedule->printBriefLofs();

	schedule->computeTerminalAircraftCount();

	Model * model = new Model(stationList, aircraftList, legList, schedule->getLofList(),schedule->getNodeList());

	//	model->populateByColumn();

	//	model->solve();

	model->populateLofByAircraft();  /// agg model��col enumeration���ģ�ͣ�����ѡ������lof���Ƹ�aircraft��ÿ��aircraft��һ��������lofList
	model->delayLofsCrossDay();  /// ÿ��aircraft��lofList��cross day delay, ÿ�ܷɻ���lofList����һ�����getOperLofList, ������modelIP��
	model->computeLofCost(); /// ÿ��aircraft��lofList��lof ����cost

	Model * modelIP = new Model(stationList,model->getAircraftList(),legList,model->getOperLofList());
	modelIP->populateByColumn();

	vector<Lof *> lofListSoln;
	lofListSoln = modelIP->solveIP();
	*/

	clock_t startTime = clock();

	Schedule * schedule = new Schedule(stationList, aircraftList, legList);
	schedule->computeTopOrder();

	Model * model = new Model(stationList, aircraftList, legList, schedule->getTopOrderList());
	cout << "top order" << endl;
	for(auto ele: schedule->getTopOrderList()) {
		ele->print();
	}
	vector<Lof *> lofListSoln;
	lofListSoln = model->solveColGen();

	vector<Leg *> finaLegList;
	finaLegList = updaInfo(lofListSoln,legList);

	cout << "Total number of connection of leg network: " << schedule->getConnectionSize() << endl;

	clock_t endTime = clock();
	cout << "total run time is " << (endTime - startTime)/CLK_TCK << " seconds" << endl;

	if (exportSolution(output_path, finaLegList)) {
		std::cout << "Solution is printed." << std::endl;
		cout << "output path: " << output_path << endl;
	}
	else {
		std::cout << "Solution not printed" << endl;
		cout << "output path: " << output_path << endl;
	}
	
	
	cout << endl;
	cout << "Canceled flights are" << endl;
	for (int i = 0; i < finaLegList.size(); i++)
	{
		if (!finaLegList[i]->getAssigned())
		{
			// �����cancel, ��ʾ������Ϣ����dual
			finaLegList[i]->print();
			cout << "dual is " << finaLegList[i]->getDual() << endl;
		}
	}

	system("pause");
	return 0;

} 
