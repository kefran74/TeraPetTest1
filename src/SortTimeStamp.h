#pragma once

#include <iostream>
#include <vector>

using namespace std;

typedef struct sorted_data {
	unsigned int timestamp;
	unsigned int energy;
} my_data;

class SortTimeStamp
{

public:
    // Constructeurs
    SortTimeStamp( char* inFileName );

    void ParseFile(void);

    char* FileName;

    

private: 

    std::vector<uint> time;   // timestamps
    std::vector<uint> energy; // energy

    // data pair to sort by time
    std::vector<my_data> output_data;

    void process( my_data* buffer, size_t len );

    void write_tsv(std::string filename, std::vector<std::pair<std::string, std::vector<uint>>> dataset);
};
