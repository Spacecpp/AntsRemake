#include "General_func.h"
#include <algorithm>

int tile_distance(int x1, int y1, int x2, int y2)
{	
	return std::max( abs(x1 - x2), abs(y1 - y2) );	
}


void log_message(const std::string & message, const std::string & filename)
{
	
	FILE * file = fopen(filename.c_str(), "a");
	if (file)
	{
		fprintf(file, "%s\n", message.c_str() );
		fclose(file);
	}
	
}
