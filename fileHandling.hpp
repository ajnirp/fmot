#ifndef fileHandlingH
#define fileHandlingH

#include <algorithm>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#include "essentials.hpp"

string& file_data( string );

// name -> name of the file we want to create
// overwrite -> if true, deletes any existing file called 'name'
//  if false (default), the function exits and returns false
bool create_blank_file( string name, bool overwrite = false );

bool create_empty_folder( string );
bool remove_file( string );

// Returns true if the file was opened successfully, false otherwise.
// file -> name of the file, or (possibly) absolute path to the file.
bool open_file( string );

bool listingFilePossible( string );
void listFilesInDirectory( vector< string >&, string& );

#endif
