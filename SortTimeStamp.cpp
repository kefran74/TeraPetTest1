#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstring>

#include "SortTimeStamp.h"

using namespace std;

// PUBLIC

// constructor
SortTimeStamp::SortTimeStamp( char* fileName )
{
    this->FileName = fileName;

    // init vector
}


void SortTimeStamp::ParseFile(  )
{
	std::ifstream ifs( this->FileName, std::ifstream::in );
	if( ifs.fail() ) throw "error opening";

	// limited buffer size to prevent memory overflow
	const std::size_t BUFFER_SIZE = 1024000;

	// following will store time / energy combinations
	char raw_input [BUFFER_SIZE];

	// parse raw input binary file
	// cout << "The raw file contains:" << endl;

	size_t nRead = 0;
	my_data* input = NULL;

    while( ifs.read ( raw_input, sizeof(raw_input)) )
	{
		// cout << "read function returned " << ifs.gcount() << endl;

		input = (my_data*)raw_input;
    
		this->Process( input, ifs.gcount() / sizeof(my_data) );
    }

    if( ! ifs.eof() ) throw "error reading";

    this->Process( input, ifs.gcount() / sizeof(my_data) );
    ifs.close();

	cout << endl;

    // print final sorted vector
    cout << "Data sorted by time stamp : \n";
    for (auto x : this->output_data)
        cout << "[" << x.timestamp << ", " << x.energy << "] " << endl;
}



// PRIVATE

bool compareTimeStamp( my_data data1, my_data data2)
{
    return ( data1.timestamp < data2.timestamp );
}

void SortTimeStamp::Process( my_data* buffer, size_t len )
{
	// cout << "process @ " << buffer << " with size " << len << endl;
    cout << ".";
	for( int i = 0; i < len; i++ )
	{
        // display new sample
		// cout << "data sample " << i << ": " << buffer[i].timestamp << "\t" << buffer[i].energy << endl;

        // add it to vector
        this->output_data.push_back(buffer[i]);
    }

    // sort vector with timestamps
    sort (this->output_data.begin(), this->output_data.end(), compareTimeStamp);

    // cout << "Data sorted by time stamp : \n";
    // for (auto x : this->output_data)
    //     cout << "[" << x.timestamp << ", " << x.energy << "] " << endl;
}


