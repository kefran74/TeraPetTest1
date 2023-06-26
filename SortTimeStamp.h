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
    vector<my_data> output_data;

private: 
    void Process( my_data* buffer, size_t len );
};
