#include <cstdlib> 
#include <cstring> 
#include <ctime> 
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include "tinyxml2.h"

using namespace std;

bool importAircarfts(const std::string& fullFileName) { 
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
		auto tailNumber = eleChild1->FirstChildElement("ns1:tailNumber")->GetText();
        cout << tailNumber << endl;

		unsigned int st = 0, et = 0;
		eleChild1->FirstChildElement( "ns1:startAvailableTime" )->QueryUnsignedText( &st ); 
		eleChild1->FirstChildElement("ns1:endAvailableTime")->QueryUnsignedText(&et);
		auto startAvailableTime = st;
		auto endAvailableTime = et;
        cout << startAvailableTime << endl;
        cout << endAvailableTime << endl;

		auto startAvailableAirport = eleChild1->FirstChildElement( "ns1:startAvailableAirport" )->GetText(); 
		auto endAvailableAirport = eleChild1->FirstChildElement( "ns1:endAvailableAirport" )->GetText(); 
        cout << startAvailableAirport << endl;
        cout << endAvailableAirport << endl;
		eleChild1 = eleChild1->NextSiblingElement();
	}
	return true;
} 

int main() {
    importAircarfts("AirCraft.xml");
    return 0;
}