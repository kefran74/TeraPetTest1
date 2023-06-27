#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstring>


#include <boost/filesystem.hpp>

#include "SortTimeStamp.h"

using namespace std;

#define MAX_BUFFER_SIZE     5000000


///////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////

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
	event_data* input = NULL;
    std::size_t buffer_size = 0;
    std::streampos fileSize = 0;
    frame_header header;

    // parse raw input binary file
	cout << "Parsing raw file ..." << endl;

	std::ifstream ifs( this->FileName, std::ifstream::in );
	if( ifs.fail() ) throw "error opening";

    // get input file size to adjust buffer
    if( ifs.is_open() ) {
        ifs.seekg(0, std::ios::end); // Move to the end of the file
        fileSize = ifs.tellg(); // Get the file size
        ifs.seekg(0, std::ios::beg); // Move back to the beginning of the file
        cout << "File size: " << fileSize << " bytes" << std::endl;
    }

	// limited buffer size to prevent memory overflow
    if ( fileSize > MAX_BUFFER_SIZE ){
        buffer_size = MAX_BUFFER_SIZE;
    }
    else
    {
        buffer_size = fileSize;
    }

    cout << "Buffer size: " << buffer_size << " bytes" << std::endl;

    char raw_input [buffer_size];

    while( ! ifs.eof() )
    {
        // read frame header
        ifs.read ( (char*)&header, sizeof(header));

        cout << endl;
        cout << ">>> Frame ID " << header.frame_Id << " contains " << header.event_nb << " events" << endl;

        // read events from this frame
        ifs.read ( raw_input, ( sizeof(event_data) * header.event_nb ));
	
		cout << "read function returned " << ifs.gcount() << " bytes" << endl;
        
        if (0 < ifs.gcount() ) 
        {
		    input = (event_data*)raw_input;
		    this->processFrame( input, header );
        }
    }

    ifs.close();

	cout << endl;

    // print and split final sorted vector
    // cout << "Data sorted by time stamp : \n";
    for (auto x : this->output_data)
    {
        // cout << "[" << x.timestamp << ", " << x.energy << "] " << endl;

        // split into 2 seperate uint vectors
        this->time.push_back( x.time );
        this->energy.push_back( x.energy );
    }

     // Wrap into a vector
    std::vector<std::pair<std::string, std::vector<uint>>> vals = { {"Time", this->time}, {"Energy", this->energy } };
    
    // Write the vector to TSV
    this->write_tsv("output.tsv", vals);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////////////////////////////


static bool compareTimeStamp( event_data data1, event_data data2)
{
    return ( data1.time < data2.time );
}

void SortTimeStamp::processFrame( event_data* buffer, frame_header header )
{
	cout << "process Frame Id " << header.frame_Id << ", with " << header.event_nb << " events" << endl;
    // cout << ".";

    if ( NULL == buffer ) {
        cout << "NULL buffer " << endl;
        return;
    }

    if( header.event_nb == 0 )
    {
        cout << "no Frames to process" << endl;
        return;
    }

	for( int i = 0; i < header.event_nb; i++ )
	{
        // display new sample
		// cout << "data sample " << i << ": " << buffer[i].timestamp << "\t" << buffer[i].energy << endl;

        // add it to vector
        this->output_data.push_back(buffer[i]);
    }

    // sort vector with timestamps
    sort (this->output_data.begin(), this->output_data.end(), compareTimeStamp);

    // cout << "Data sorted by time stamp : \n";
    // for (auto x : this->output_data) {
    //     cout << "[" << x.timestamp << ", " << x.energy << "] " << endl;
    // }
}


void SortTimeStamp::write_tsv(std::string filename, std::vector<std::pair<std::string, std::vector<uint>>> dataset)
{
    // Make a TSV file with 2 columns of integer values
    // Each column of data is represented by the pair <column name, column data>
    //   as std::pair<std::string, std::vector<uint>>
    // The dataset is represented as a vector of these columns
    // Note that all columns should be the same size

    cout << "write_tsv file " << filename << " with " << dataset.size() << " columns" << endl;
    
    // Create an output filestream object
    std::ofstream myFile(filename);
    
    // Send column names to the stream
    for(int j = 0; j < dataset.size(); ++j)
    {
        myFile << dataset.at(j).first;
        if(j != dataset.size() - 1) myFile << "\t"; // No tab at end of line
    }
    myFile << "\n";
    
    // Send data to the stream
    for(int i = 0; i < dataset.at(0).second.size(); ++i)
    {
        for(int j = 0; j < dataset.size(); ++j)
        {
            myFile << dataset.at(j).second.at(i);
            if(j != dataset.size() - 1) myFile << "\t"; // No tab at end of line
        }
        myFile << "\n";
    }
    
    // Close the file
    myFile.close();
}
