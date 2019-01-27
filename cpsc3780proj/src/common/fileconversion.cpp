// author Thomas Richardson
// date Nov 28, 2018
//
// 
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "fileconversion.h"
#include "UsefulFunctions.cpp"

bool filetostrings(std::string file, std::vector<std::string>& vecOfStrs){
	// Open the File
	std::ifstream in(file.c_str());
	// Check if object is valid
	if(!in)
	{
		std::cerr << "Cannot open the File : "<<file<<std::endl;
		return false;
	}
	std::string str;
	// Read the next line from File untill it reaches the end.
	while (std::getline(in, str))
	{
		// Line contains string of length > 0 then save it in vector
		if(str.size() > 0)
			vecOfStrs.push_back(str);
	}
	//Close The File
	in.close();
	return true;
}
std::vector<std::string> stringtoAscii(std::vector<std::string>& vecOfStrs, std::vector<std::string>& vecOfStrs2){
    int check1 = vecOfStrs.size();
    for(int i = 0; i < check1; i++)
    {
        std::string temp = vecOfStrs[i];
        int check2 = temp.size();
        //insert a size check for 64 char sets
        std::string convertedLine;
        for(int j = 0; j < check2; j++)
        {
            char temp2 = temp[j];
            int temp3 = temp2;
            convertedLine += temp3; 
        }
        vecOfStrs2.push_back(convertedLine);
    }
    return vecOfStrs2;
}