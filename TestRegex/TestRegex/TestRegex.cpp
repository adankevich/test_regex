// TestRegex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <array>

enum Replacements {
	ID = 0,
	NAME,
	RESULT,
	TYPE,
};

struct Rule
{
	std::string raw_str;	// "Unmatched - $ID$ - $TYPE$"
	std::string regex_str;	// "Unmatched - (.*) - (.*)"
	std::vector<std::string> replacements;	// { "$ID$", "$TYPE$" }

	Rule ( std::string const& rule_str )
	{
		this->raw_str = rule_str;

		std::regex const rx ("(\\$)([^ ]*)"); // match words starting with $ charachter

		std::smatch sm;
		std::string str = rule_str; 
		while (std::regex_search ( str, sm, rx)) {

			std::cout << sm[0];
			std::cout << std::endl;

			this->replacements.push_back ( sm[0] ); // is it reliable to use 0th index???

			str = sm.suffix().str();
		}

		this->regex_str = std::regex_replace ( rule_str, rx, "(.*)" );

		std::cout << std::endl << this->regex_str << std::endl;

	}
};

//std::vector<std::string> read_log ()
//{
//}

std::vector<Rule> read_rules ()
{
	std::array<std::string, 2> rule_strings = {
		"Unmatched - $ID$ - $TYPE$",
		"Unmatched - $TYPE$ - $ID$"
	};

	std::vector<Rule> rules;
	rules.reserve ( rule_strings.size() );
	for ( auto const& str : rule_strings )
		rules.push_back ( str );

	return rules;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::vector<Rule> rules = read_rules ();

	//std::regex const rx ( rule.regex_str );
	//std::smatch sm;

	std::array<std::string, 2> error_log = {
		"Unmatched - Part Reference - Reference Name"
	};
	
	//if ( std::regex_match ( input_string, sm, rx ) )
	//	for ( size_t i = 1; i < sm.size() ; ++i )
	//			std::cout << sm[i] << std::endl;

	std::cout << std::endl;

	return 0;
}

