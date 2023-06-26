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
    cout << "Constructor uses dat file " << fileName << endl;


    // check if file exists
    boost::filesystem::path myfile(fileName);

    if( !boost::filesystem::exists(myfile) )
    {
        cout << "file does not exists" << endl;
        throw EEXIST;
        this->FileName = NULL;
    }
    else
        this->FileName = fileName;
}



void SortTimeStamp::ParseFile( void )
{
    size_t nRead = 0;
	my_data* input = NULL;

    // parse raw input binary file
	cout << "Parsing raw file ..." << endl;

	std::ifstream ifs( this->FileName, std::ifstream::in );
	if( ifs.fail() ) throw "error opening";

	// limited buffer size to prevent memory overflow
	const std::size_t BUFFER_SIZE = 1024;

	// following will store time / energy combinations
	char raw_input [BUFFER_SIZE];

    while( ifs.read ( raw_input, sizeof(raw_input)) )
	{
		cout << "read function returned " << ifs.gcount() << endl;

		input = (my_data*)raw_input;
    
		this->process( input, ifs.gcount() / sizeof(my_data) );
    }

    if( ! ifs.eof() ) throw "error reading";

    this->process( input, ifs.gcount() / sizeof(my_data) );

    ifs.close();

	cout << endl;

    // print final sorted vector
    cout << "Data sorted by time stamp : \n";
    for (auto x : this->output_data)
        cout << "[" << x.timestamp << ", " << x.energy << "] " << endl;

     // Wrap into a vector
    std::vector<std::pair<std::string, std::vector<int>>> vals = {{"TimeStamp", vec1}, {"Two", vec2}, {"Three", vec3}};
    
    // Write the vector to CSV
    write_csv("two_cols.tsv", vals);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////////////////////////////


bool compareTimeStamp( my_data data1, my_data data2)
{
    return ( data1.timestamp < data2.timestamp );
}

void SortTimeStamp::process( my_data* buffer, size_t len )
{
	cout << "process @ " << buffer << " with size " << len << endl;
    // cout << ".";

	for( int i = 0; i < len; i++ )
	{
        // display new sample
		cout << "data sample " << i << ": " << buffer[i].timestamp << "\t" << buffer[i].energy << endl;

        // add it to vector
        this->output_data.push_back(buffer[i]);
    }

    // sort vector with timestamps
    sort (this->output_data.begin(), this->output_data.end(), compareTimeStamp);

    cout << "Data sorted by time stamp : \n";
    for (auto x : this->output_data)
        cout << "[" << x.timestamp << ", " << x.energy << "] " << endl;
}


void SortTimeStamp::write_csv(std::string filename, std::vector<std::pair<std::string, std::vector<uint>>> dataset){
    // Make a CSV file with 2 columns of integer values
    // Each column of data is represented by the pair <column name, column data>
    //   as std::pair<std::string, std::vector<int>>
    // The dataset is represented as a vector of these columns
    // Note that all columns should be the same size
    
    // Create an output filestream object
    std::ofstream myFile(filename);
    
    // Send column names to the stream
    for(int j = 0; j < dataset.size(); ++j)
    {
        myFile << dataset.at(j).first;
        if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
    }
    myFile << "\n";
    
    // Send data to the stream
    for(int i = 0; i < dataset.at(0).second.size(); ++i)
    {
        for(int j = 0; j < dataset.size(); ++j)
        {
            myFile << dataset.at(j).second.at(i);
            if(j != dataset.size() - 1) myFile << ","; // No comma at end of line
        }
        myFile << "\n";
    }
    
    // Close the file
    myFile.close();
}
