#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstring>


#include <boost/filesystem.hpp>

#include "SortTimeStamp.h"

using namespace std;

using namespace boost;


// DEFINES
//////////////////////////////////////////////////////////

#define MAX_EVENTS_PER_FRAME     1023
#define MAX_BUFFER_SIZE         5000000

/**
 * forward declaration to use 
*/
static bool compareTimeStamp( event_data data1, event_data data2);


///////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////


/** 
 * constructor
 * 
 \param[in]  input data file to sort

 \returns   class instantiation
*/
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


/** 
 * ParseFile: get raw data from file, sort events by timestamps
 * 
 \param[in]  none

 \returns   none
*/
void SortTimeStamp::LoadRawData( void )
{
	// store Frame header and events
    frame_header header;
    event_data* input = NULL;

    // used to handle file chunk size and frame size
    std::size_t raw_buffer_size = 0;
    std::size_t frame_size = 0;
    std::size_t file_chunk_size = 0;

    // parse raw input binary file
	cout << "Parsing input data file ..." << endl;

	std::ifstream ifs( this->FileName, std::ifstream::in );
	if( ifs.fail() ) throw "error opening";

    // get input file size to adjust buffer
    if( ifs.is_open() ) {
        ifs.seekg(0, std::ios::end); // Move to the end of the file
        this->fileSize = ifs.tellg(); // Get the file size
        ifs.seekg(0, std::ios::beg); // Move back to the beginning of the file
        cout << "### File size: " << fileSize << " bytes ###" << std::endl;
    }

    // read max between file size and max buffer size
    if( fileSize < MAX_BUFFER_SIZE )
    {
        raw_buffer_size = fileSize;
    }
    else
    {
        raw_buffer_size = MAX_BUFFER_SIZE;
    }

    // hold a chunk of file
    char raw_input [raw_buffer_size];

    // point to the current frame
    char* frame = NULL;

    // read file in chunks
    while( this->remainingBytesToRead )
	{
        // check file remaining size
        this->currentPosition = ifs.tellg();

        this->remainingBytesToRead = this->fileSize - this->currentPosition;

        cout << endl;
        cout << "current position " << this->currentPosition << ", " << this->remainingBytesToRead << " remaining Bytes to Read" << endl;

        // read accordingly
        if( this->remainingBytesToRead > raw_buffer_size )
        {
            ifs.read ( raw_input, raw_buffer_size );
        }
        else
        {
            ifs.read ( raw_input, this->remainingBytesToRead );
        }

        file_chunk_size = ifs.gcount();

		cout << "read function returned " << file_chunk_size << " bytes, current position " << currentPosition << endl;

        if ( file_chunk_size == 0 ) break;
        
        uint i = 0;

        // poll through each frame within file chunk
        while( i < file_chunk_size )
        {
            // cout << "Current Frame Pointer = " << i << endl;

            // update frame pointer
            frame = raw_input+i;

            // check frame header
            std::memcpy((char*)&header, frame, sizeof(header));
            // cout << "Frame ID " << header.frame_Id << " contains " << header.event_nb << " events" << endl;

            frame_size = header.event_nb * sizeof(event_data);

            // cout << "Frame Size " << frame_size << " Bytes" << endl;

            // if this file chunk contains the entire frame, process it
            if( ( file_chunk_size - i ) > frame_size )
            {
                // process Frame
                input = (event_data*)(frame+sizeof(header));
	 	        this->processFrame( input, header );
                // update frame pointer
                i += frame_size + sizeof(header);
            }
            else
            {
                // otherwise rewind the file pointer, so the next file chunk starts exactly at the next frame header
                cout << "rewind last " << ( file_chunk_size - i ) << " bytes of this file chunk" << endl;

                this->nextPosition = this->currentPosition + i ;

                cout << "seek to " << this->nextPosition << " to read next header" << endl;

                ifs.seekg( this->nextPosition );
                // exit this chunk loop
                break;
            }
        } // while(  i < file_chunk_size )

        cout << "End of chunk, " << i << " bytes processed" << endl;

    }   // while ( remainingBytesToRead )

    ifs.close();

}


/** 
 * ParseFile: get raw data from file, sort events by timestamps
 * 
 \param[in]  none

 \returns   none
*/
void SortTimeStamp::SortEvents( void )
{

	cout << " Sort vector" << endl;

    // sort vector with timestamps
    sort (this->output_data.begin(), this->output_data.end(), compareTimeStamp);

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
    this->writeTsv("output.tsv", vals);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////////////////////////////

/** 
 * compareTimeStamp: static method used by the sort method used to process frames
 * 
 \param[in]  event_data data1: a first data tuple that concatenate timestamp and corresponding energy

 \param[in]  event_data data2: a second data tuple that concatenate timestamp and corresponding energy

 \returns   true if event 1 timestamp smaller than event 2, false otherwise
*/
static bool compareTimeStamp( event_data data1, event_data data2)
{
    return ( data1.time < data2.time );
}


/** 
 * processFrame: process a list of events and store them in a vector for post processing.
 * 
 \param[in]  event_data* buffer: a pointer to a list of events contained in a frame

 \param[in]  frame_header header:frame header to indicate Frame ID and size
*/
void SortTimeStamp::processFrame( event_data* buffer, frame_header header )
{
	cout << "process Frame Id " << header.frame_Id << ", with " << header.event_nb << " events" << endl;
    //cout << ".";

    if ( NULL == buffer ) {
        cout << "NULL buffer " << endl;
        return;
    }

    if( header.event_nb == 0 )
    {
        cout << "no Frames to process" << endl;
        return;
    }

    // fill data vector
	for( int i = 0; i < header.event_nb; i++ )
	{
        // display new sample
		// cout << "data sample " << i << ": " << buffer[i].timestamp << "\t" << buffer[i].energy << endl;

        // add it to vector
        this->output_data.push_back(buffer[i]);
    }
}


/** 
 * writeTsv: store the output data in tsv file.
 * 
 \param[in] std::string filename: output file name to store result

 \param[in]  dataset: vector formatted output data to be written in tsv file.
*/
void SortTimeStamp::writeTsv(std::string filename, std::vector<std::pair<std::string, std::vector<uint>>> dataset)
{
    // Make a TSV file with 2 columns of integer values
    // Each column of data is represented by the pair <column name, column data>
    //   as std::pair<std::string, std::vector<uint>>
    // The dataset is represented as a vector of these columns
    // Note that all columns should be the same size

    cout << "writeTsv file " << filename << " with " << dataset.size() << " columns" << endl;
    
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
