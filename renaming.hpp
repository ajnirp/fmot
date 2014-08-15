#ifndef renamingH
#define renamingH

#include <string>
#include <vector>
using namespace std;

#include "essentials.hpp"

// file names => file (absolute) paths (here!)
typedef vector< string > list_of_files;

int rename_files( list_of_files& , list_of_files& );
bool rename_file( string , string );
string create_backup( string );

#endif
