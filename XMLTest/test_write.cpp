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

int main() {
	XMLDocument doc;
	XMLElement* root=doc.NewElement("DBUSER");
	doc.InsertEndChild(root);
	XMLText *test1 = doc.NewText("test words");
	root->InsertEndChild(test1);
	XMLElement *test2 = doc.NewElement("test2");
	root->InsertEndChild(test2);
	doc.SaveFile("write_cpp.xml");
	return 0;
}