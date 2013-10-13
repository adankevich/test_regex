#include "stdafx.h"

#include <cassert>
#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <array>
#include <unordered_set>

struct Rule;

typedef std::vector<Rule> Rules_cont;
typedef std::unordered_set<std::string> Results_cont;
typedef std::unordered_set<std::string> Types_cont;

Rules_cont rules;
Results_cont results;
Types_cont types;

enum Replacement {
	INVALID = -1,
	ID = 0,
	NAME,
	RESULT,
	TYPE,
};

bool is_valid_id ( std::string const& value )
{
	std::regex const rx ( "\\d+|\\d+\\s*\\(\\d+\\)" );

	return std::regex_match ( value, rx );
}

bool is_valid_result ( std::string const& value )
{
	return ( results.find ( value ) != results.end () );
}

bool is_valid_type ( std::string const& value )
{
	return ( types.find ( value ) != types.end () );
}

bool is_value_correct ( std::string const& value, Replacement const replacement )
{
	bool result = true;

	switch ( replacement )
	{
	case ID:
		result = is_valid_id ( value );
		break;

	case RESULT:
		result = is_valid_result ( value );
		break;

	case TYPE:
		result = is_valid_type ( value );
		break;
	}

	return result;
}

Replacement string2replacement ( std::string const& str )
{
	auto result = INVALID;

	if ( str == "ID" )
		result = ID;
	else if ( str == "NAME" )
		result = NAME;
	else if ( str == "RESULT" )
		result = RESULT;	
	else if ( str == "TYPE" )
		result = TYPE;

	return result;
}


struct ErrorData
{
	std::string original;
	std::string id;
	std::string type;
	std::string result;
	std::string rule_str;
	bool valid;
};

struct Rule
{
	std::string raw_str;	// "%Unmatched% - $ID$ - $TYPE$"
	std::string regex_str;	// "%Unmatched% - (.*) - (.*)". Shouldn't be used directly, use localized() instead
	std::vector<Replacement> replacements;	// { ID, TYPE }

	Rule ( std::string const& rule_str )
	{
		this->raw_str = rule_str;

		std::regex const rx ("\\$(\\w+)\\$"); // match word surrounded with $ charachter. Results { $\w$, \w }

		std::smatch sm;
		std::string str = rule_str;
		while (std::regex_search ( str, sm, rx)) {

			//std::cout << sm[0];
			//std::cout << std::endl;

			this->replacements.push_back ( string2replacement ( sm[1] )); // is it reliable to use 1st index???

			str = sm.suffix().str();
		}

		this->regex_str = std::regex_replace ( rule_str, rx, "(.*)" );

		//std::cout << std::endl << this->regex_str << std::endl;

	}

	ErrorData process_error ( std::string const& error_str ) const
	{
		ErrorData err_data;
		err_data.valid = false;

		std::regex const rx ( localized () );
		std::smatch sm;

		if ( std::regex_match ( error_str, sm, rx ) && (sm.size() - 1) == replacements.size () )
		{
			for ( size_t i = 1; i < sm.size() ; ++i )
			{
				auto const& value = sm[i].str ();
				auto replacement = replacements[i-1];

				if ( !is_value_correct ( value, replacement ) ) // current rule doesn't match
				{
					err_data.valid = false;
					break; 
				}

				err_data.valid = true;
				switch ( replacement )
				{
				case ID:
					err_data.id += value + "; ";
					break;

				case RESULT:
					err_data.result = value;
					break;

				case TYPE:
					err_data.type = value;
					break;
				}
			}

			if ( err_data.valid )
			{
				err_data.original = error_str;
				err_data.rule_str = raw_str;
			}
		}

		return err_data;
	}

	std::string localized ( ) const
	{
		assert ( !regex_str.empty () );


		std::regex const rx ("%(\\w*)%"); // match word surrounded with % charachter. Results { %\w%, \w }
		std::smatch sm;

		// Retrieve strings to be localized
		std::vector<std::string> tokens;
		std::vector<std::string> loc_strings;
		std::string result = regex_str;
		while ( std::regex_search ( result, sm, rx ) )
		{
			assert ( sm.size() >= 2 );

			tokens.push_back ( sm[0] );
			loc_strings.push_back ( sm[1] ); // Fake localization. To be implemented.
			result = sm.suffix().str();
		}

		// Replace with localized strings
		result = regex_str;
		for ( size_t i = 0; i < tokens.size(); i++ )
		{
			std::regex rx ( tokens[i] );
			std::string tmp;
			result = std::regex_replace (result, rx, loc_strings[i]);
		}

		return result;
	}
};

// Should be read from TYPES.txt
Types_cont read_types ()
{
	std::string tmp[] = { "Solid Face", "Parabola", "Point", "Part Reference"};
	Types_cont types (tmp, tmp + sizeof(tmp) / sizeof(tmp[0]));

	return types;
}

// Should be read from RESULTS.txt
Results_cont read_results ()
{
	std::string tmp[] = { "Geometric Face", "Reference Name", "Trim Edge"};
	Results_cont results (tmp, tmp + sizeof(tmp) / sizeof(tmp[0]));

	return results;
}

std::vector<Rule> read_rules ()
{
	std::array<std::string, 7> rule_strings = {
		"%Unmatched% %Ent% - $ID$ - $RESULT$",
		"%Unmatched% - $ID$ - $TYPE$",
		"%Unmatched% - $ID$ - $ID$ - $TYPE$",
		"%Unmatched% - $ID$ - $ID$ - $TYPE$ - $RESULT$",
		"%Unmatched% - $ID$ - $RESULT$",
		"%Unmatched% - $TYPE$ - $RESULT$",
		"%Unmatched% - $RESULT$ - $TYPE$",
	};

	std::vector<Rule> rules;
	rules.reserve ( rule_strings.size() );
	for ( auto const& str : rule_strings )
		rules.push_back ( str );

	return rules;
}

void init ()
{
	rules = read_rules ();
	types = read_types ();
	results = read_results ();

	// Print all rules
	std::cout << "Rules:" << std::endl;
	for ( auto const& rule : rules )
		std::cout << rule.raw_str << std::endl;
	std::cout << std::endl;
}

int main()
{
	init ();

	std::array<std::string, 7> error_strings = {
		"Unmatched Ent - 12 - Reference Name", // ID - Result
		"Unmatched - 12 (10) - Reference Name", // ID - Result
		"Unmatched - 12 (10) - 42 - Part Reference", // ID - ID - Type
		"Unmatched - 42 - 12 (10) - Part Reference - Reference Name", // ID - ID - Type - Result
		"Unmatched - Part Reference - Reference Name", // Type - Result
		"Unmatched - Reference Name - Part Reference", // Result - Type
		"Unmatched - Reference Name - Part Reference - Reference Name", // UNMATCHED
	};

	std::cout << "Start parsing..." << std::endl;
	for ( auto const& error_str : error_strings )
	{
		std::cout << "Error line: " << error_str << std::endl;
		ErrorData err_data;
		for ( auto const& rule : rules )
		{
			err_data = rule.process_error ( error_str );
			if ( err_data.valid )
				break;
		} // iterate rules

		if ( err_data.valid )
		{
			// Found correct Rule
			// print results
			std::cout << "MATCHED:" << std::endl;
			std::cout << "\t Rule: " << err_data.rule_str << std::endl;
			std::cout << std::endl;
			std::cout << "\t ID: " << err_data.id << std::endl;
			std::cout << "\t TYPE: " << err_data.type << std::endl;
			std::cout << "\t RESULT: " << err_data.result << std::endl;
		}
		else
			std::cout << "NO MATCHING RULE FOUND" << std::endl;
		
		std::cout << std::endl;
	} // iterate errors

	std::cout << std::endl;

	return 0;
}

