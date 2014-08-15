#include "luaSearching.hpp"

void simpleSearch( string dirname, string pattern, vector< string >& results, bool shallow ){

	DIR *dp;
	dirent *ep;
	dp = opendir( dirname );
	if( dp != NULL ){

		vector< string > files;
		while( ep = readdir( dp ) )
			files.push_back( ep->d_name );
		(void) closedir(dp);

		for( auto itr = files.begin( ) ; itr != files.end( ) ; itr++ ){

			if( *itr == "." or *itr == ".." )
				continue;

			if( match( *itr, pattern ) )
				//char currDir[ 256 ]; getcwd( currDir, 256 );
				results.push_back( addToPath( string( dirname ), *itr ) );
				//make_pair( ( string ) *itr + " in " + string( currDir ), -1 )

			// Recursively search the file, as it might be a directory.
			if( !shallow ) simpleSearch( addToPath( dirname, *itr ), pattern, results, false );

		}

	}

}
