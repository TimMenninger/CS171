/******************************************************************************

 utils.cpp

 Contains utilities to help the readability and functionality of transformation
 code.

 Author: Tim Menninger

******************************************************************************/

#include "utils.h"

using namespace std;

/*
 getSpaceDelimitedWords

 Takes a line and returns a vector of words in that line, which are assumed
 to be separated by any amount of whitespace.

 Arguments: string line - The line to parse

 Returns:   vector<string> - A vector of words in the argued line
*/
vector<string> getSpaceDelimitedWords
(
    string line
)
{
    // Split the space-separated line into a vector of strings
    vector<string> vals;
    string temp;
    stringstream s(line);
    while (s >> temp)
        vals.push_back(temp);

    return vals;
}
