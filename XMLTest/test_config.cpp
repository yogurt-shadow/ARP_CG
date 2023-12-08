#include <cstdlib> 
#include <cstring> 
#include <ctime> 
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

using namespace std;

void readConfiguarationFile(std::string& input_dir, std::string& output_dir)
{
	std::string configFile = "./Config.txt";
	std::ifstream inputFile(configFile.c_str());
	if (!inputFile)
	{
        cout << "no config file" << endl;
		return;
	}
	std::string fieldName[100];
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
			i++;
			continue;
		}

		if (fieldName[i] == "OUTPUT_DIRECTORY")
		{
			inputFile >> output_dir;
			output_dir += "\\";
			i++;
			continue;
		}

		i++;
		if (i == 2)
			break;
	}
	inputFile.close();
	return;
}

int main() {
    string input, output;
    readConfiguarationFile(input, output);
    cout << "input: " << input << endl;
    cout << "output: " << output << endl;
    return 0;
}