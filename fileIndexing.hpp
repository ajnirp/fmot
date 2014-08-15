#ifndef fileIndexingH
#define fileIndexingH

#include "matchWithLua.hpp"
#include "generalTree.hpp"

#include <dirent.h>
#include <fstream>
#include <string>
using namespace std;

#include "essentials.hpp"

class database{

	private:

	string file_name, curr_dir;

	void add_to_tree( tree< string >&, node< string >&, string );
	void layered_search( node< string >*, vector< string >&, string& );

	public:

	bool refresh;

	explicit database( string dir_path = string( "" ), /**/
		/**/ string db_name = string( "database" ), bool re_make = false );
	bool build( string dir_path, bool forced = false );
	vector< string >& search( string );

};

#endif
