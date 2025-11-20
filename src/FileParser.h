#ifndef FILEPARSER_H
#define FILEPARSER_H

#include <stdio.h>
#include <string>

// class to easily read key and value strings separated by an '=', line by line
class FileParser
{
	
	private:
		FILE * cfile;
		
		void parse_line();
		bool equal_found;
	
	public:
		std::string key, value, line;
		int linenumber;
		
		FileParser();
		FileParser(FILE * file);
		void set(FILE * file);
		void clear_current_line();
		
		bool end_of_file();
		bool error();
		bool key_and_value_ok();
		bool empty_line();
		bool key_missing_value();
		
		bool get_next_line();

};
#endif
