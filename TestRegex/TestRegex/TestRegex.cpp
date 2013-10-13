// TestRegex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <regex>

bool parse_match ( std::smatch::const_reference subm, const size_t orig_length )
{
	if ( orig_length == subm.length () )
		return false;

	const auto& s = subm.str ();
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::string sentence ("Unmatched - /ID/ - /TYPE/ - /TYPE/");
	std::regex e ("Unmatched - (.*) - (.*) - (.*)");
	std::smatch sm;

	if ( std::regex_match ( sentence, sm, e ) ) {
		for ( size_t i = 0; i < sm.size() ; ++i ) {

			if ( parse_match ( sm[i], sentence.length () ) )
				std::cout << sm[i] << std::endl;
		}
	}

	std::cout << std::endl;

	return 0;
}

