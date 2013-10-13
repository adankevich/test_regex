// TestRegex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <regex>

int _tmain(int argc, _TCHAR* argv[])
{
	std::string sentence ("Unmatched - /ID/ - /TYPE/ - /TYPE/");
	std::regex e ("Unmatched - (.*) - (.*) - (.*)");
	std::smatch sm;

	if ( std::regex_match ( sentence, sm, e ) )
		for ( size_t i = 1; i < sm.size() ; ++i )
				std::cout << sm[i] << std::endl;

	std::cout << std::endl;

	return 0;
}

