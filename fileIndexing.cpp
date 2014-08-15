#include "fileIndexing.hpp"

void database::add_to_tree( tree< string >& t, node< string >& currNode, string dir_path ){

	DIR *dp;
	dirent *ep;
	dp = opendir( dir_path );

	if ( dp != NULL ) {

		vector< string > files;
		while( ep = readdir( dp ) )
			if( ep->d_name != string( "." ) and ep->d_name != string( ".." ) )
				files.push_back( ep->d_name );
		(void) closedir( dp );

		for( auto itr = files.begin( ) ; itr != files.end( ) ; itr++ ){
			if( *itr != string( "." ) and *itr != string( ".." ) ){
				t.addChild( currNode, *itr );
				add_to_tree( t, *( t.getLastChild( currNode ) ), addToPath( dir_path, *itr ) );
			}
		}

	}
}

void database::layered_search( node< string >* currNode, vector< string >& res, string& pattern ){

	if( currNode != NULL ){

		if( match( currNode->value, pattern ) )
			res.push_back( addToPath( curr_dir, returnPath( currNode ) ) );
		layered_search( currNode->rightSibling, res, pattern );
		layered_search( currNode->leftChild, res, pattern );

	}

}

database::database( string dir_path, string db_name, bool re_make ){

	file_name = db_name;
	refresh = re_make;
	build( dir_path );
	curr_dir = dir_path;

}

bool database::build( string dir_path, bool forced ){

	if( curr_dir == dir_path and !forced )
		return false;

	if( dir_path == string( "" ) )
		return false;

	ofstream db_file;

	// Extract the name of the directory from its path (if any).
	int i = dir_path.size( )-1;
	while( dir_path[ i ] != '/' and i != 0 ) i--;
	string dir_name = dir_path.substr( i+1, dir_path.size( )-1 );

	tree< string > t;
	t.setElement( t.root( ), "" );
	add_to_tree( t, t.root( ), dir_path );

	db_file.open( file_name.c_str( ) );
	db_file << t.serializeTree( ) << endl;

	curr_dir = dir_path;
	refresh = false;

	return true;

}

vector< string >& database::search( string pattern ){

	FILE* db = fopen( file_name.c_str( ), "r" );
	if( refresh or db == NULL )
		build( curr_dir );
	if( db == NULL )
		db = fopen( file_name.c_str( ), "r" );

	vector< string > *results = new vector< string >;

	if( db != NULL ){

		string serialized_tree_from_db;
		for( char c = getc( db ); c != EOF and c != '\n'; c = getc( db ) )
			serialized_tree_from_db.push_back( c );

		tree< string > t;
		t.deserialize( serialized_tree_from_db );

		layered_search( &( t.root( ) ), *results, pattern );

	}

	return *results;

}
