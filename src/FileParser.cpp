#include "FileParser.h"
#include "Alegrinho.h"


static bool string_empty_or_whitespaces(const std::string & str)
{
	
	if (!str.empty() )
	{
		
		for (std::string::const_iterator it = str.begin(); it != str.end(); ++it )
		{
			if (!isspace( (unsigned char)(*it) ) )
				return false;
		}
		
	}
	return true;
	
}


FileParser::FileParser(): cfile(NULL), equal_found(false), linenumber(0)
{
}

FileParser::FileParser(FILE * file)
{
	set(file);
}


void FileParser::set(FILE * file)
{
	
	this->cfile = file;
	linenumber = 0;
	equal_found = false;
	
}



void FileParser::clear_current_line()
{
	
	equal_found = false;
	key.clear();
	value.clear();
	line.clear();
	
}



bool FileParser::end_of_file()
{
	
	if (cfile)
		return feof(cfile);
	
	return false;
	
}


bool FileParser::error()
{
	
	if (cfile)
		return ferror(cfile);
	
	return false;
	
}



bool FileParser::key_and_value_ok()
{
	return ( !(key.empty() || value.empty() ) );
}


bool FileParser::empty_line()
{
	return (string_empty_or_whitespaces(key) && string_empty_or_whitespaces(value) );
}


bool FileParser::key_missing_value()
{
	return ( equal_found && !key.empty() && string_empty_or_whitespaces(value) );
}



bool FileParser::get_next_line()
{
	
	clear_current_line();
	
	char linec[255];
	
	if (cfile)
	{
		
		if (fgets(linec, 255, cfile) )
		{
			line = linec;
			parse_line();
		}
		else
			return false;
		
	}
	else return false;
	
	/*
	std::string line(linec);
	
	// apaga a quebra de linha
	std::size_t endline_index = line.find("\r\n");
	if (endline_index != std::string::npos)
		line.erase(endline_index);
	
	else 
	{
		endline_index = line.find("\n");
		if (endline_index != std::string::npos)
			line.erase(endline_index);
	}
	
	
	// procura o '=' divisor para pegar a chave
	size_t equal_index = line.find('=');
	if (equal_index != std::string::npos)
	{
		
		key = std::string(line, 0, equal_index);
		const size_t key_length = key.length();
		
		if (key_length > 0)
		{
			
			// apaga o espaco no fim
			if (key[key_length - 1] == ' ')
				key.erase(key_length - 1, 1);
			
			// pega o valor
			if (line.length() > equal_index + 1)
			{
				
				value = std::string(line, equal_index + 1);
				
				// apaga o espaco no comeco
				if (value[0] == ' ')
					value.erase(0, 1);
			
			}
			
		}
		
	}
	else key = line;
	*/
	
	linenumber += 1;
	
	return true;
	
}


/*
// static
static void trim(std::string & str)
{
	
	if (!str.empty() )
	{
		
		const char * whiteSpace = " \t\v\r\n\f";
		std::size_t start = str.find_first_not_of(whiteSpace);
		std::size_t end = str.find_last_not_of(whiteSpace);
		
		if (start == std::string::npos && end == std::string::npos) // spaces only
			str.clear();
			
		else if (start != end) // trim
			str = str.substr(start, end - start + 1); 
		
	}
	
}*/



// private
void FileParser::parse_line()
{
	
	// procura o '=' divisor para pegar a chave
	size_t equal_index = line.find('=');
	if (equal_index != std::string::npos)
	{
		
		equal_found = true;
		
		key = std::string(line, 0, equal_index);
		const size_t key_length = key.length();
		
		if (key_length > 0)
		{
			
			// pega o valor
			if (line.length() > equal_index + 1)
				value = std::string(line, equal_index + 1);
			
		}
		
		// remove white spaces
		trim(key);
		trim(value);
		
	}
	
	else 
	{
		key = line;
		trim(key);
	}
	
}
