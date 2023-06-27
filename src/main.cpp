

#include "SortTimeStamp.h"

using namespace std;


int main (int argc, char* args[] ) 
{
	if( 0 == argc ) throw "missing argument: input raw data file";
	
	SortTimeStamp* my_sort = new SortTimeStamp( args[1] );

	my_sort->ParseFile();

}