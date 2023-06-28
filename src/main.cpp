

#include "SortTimeStamp.h"

using namespace std;


int main (int argc, char* args[] ) 
{
	if( 0 == argc ) throw "missing argument: input raw data file";
	
	SortTimeStamp* my_sort = new SortTimeStamp( args[1] );

	// Task 1: Loading the raw data
	my_sort->LoadRawData();

	// Task 2: Sorting events by timestamp, into a tsv file.
	my_sort->SortEvents();

}