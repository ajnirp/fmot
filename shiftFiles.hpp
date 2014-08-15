#ifndef shiftFilesH
#define shiftFilesH

#include <string>
#include <vector>
using namespace std;

#include "essentials.hpp"

//command -> either "mv" or "cp"
//file_paths -> a vector of files (by absolute paths) to shift to destination
//destination -> the directory where we want to shift the files
bool shift_files( string command, vector< string >& file_paths, string destination );

#endif
