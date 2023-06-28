#pragma once

#include <iostream>
#include <vector>

using namespace std;

typedef struct header {
	uint32_t event_nb;
	uint32_t frame_Id;
} frame_header;

typedef struct event {
	uint32_t time;
	uint32_t energy;
} event_data;

class SortTimeStamp
{

public:
    // Constructor
    SortTimeStamp( char* inFileName );

    void LoadRawData( void );

    void SortEvents( void );

private: 

    // used to handle file chunks
    std::streampos fileSize = 0;
    std::streampos currentPosition = 0;
    std::streampos nextPosition = 0;
    std::streampos remainingBytesToRead = 1;

    // data vectors
    std::vector<uint32_t> time;   // timestamps
    std::vector<uint32_t> energy; // energy

    // data pair to sort by time
    std::vector<event_data> output_data;

    char* FileName = NULL;

    void processFrame( event_data* buffer, frame_header header );

    void writeTsv(std::string filename, std::vector<std::pair<std::string, std::vector<uint>>> dataset);
};
